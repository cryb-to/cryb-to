/*
 * Copyright (c) 2014 Dag-Erling Smørgrav
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "cryb/impl.h"

#include <sys/param.h>
#include <sys/mman.h>

#ifndef MAP_NOCORE
#define MAP_NOCORE 0
#endif

#ifndef MAP_NOSYNC
#define MAP_NOSYNC 0
#endif

#if HAVE_UTRACE
#if HAVE_SYS_KTRACE_H
#if HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#include <sys/ktrace.h>
#endif
#endif

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cryb/test.h>

/*
 * Very simple, non-thread-safe malloc() implementation tailored for unit
 * tests.  The most important feature of this implementation is the
 * t_malloc_fail flag, which can be used to force malloc(), calloc() and
 * realloc() calls to fail.  It also emits jemalloc-compatible trace
 * records on platforms that have utrace(2).
 *
 * Allocations are satisfied either from a bucket or by direct mapping.
 * The allocation size is first rounded to the nearest power of two or 16,
 * whichever is largest.  If this number is larger than the maximum bucket
 * block size, a direct mapping is used.  Otherwise, the appropriate
 * bucket is selected and the first free block from that bucket is
 * returned.  If there are no free blocks in the bucket, the allocation
 * fails.
 *
 * Bucket metadata is stored in a static array; the buckets themselves are
 * allocated using mmap().  The free list is maintained by keeping a
 * pointer to the first free block in the bucket metadata, and storing a
 * pointer to the next free block at the start of each free block.  These
 * pointers are not preinitialized, which avoid faulting in large amounts
 * of memory that will never be used.
 *
 * Direct mappings are allocated using mmap().  Metadata for each mapping
 * is stored in a malloc()ed struct in a linked list.
 *
 * Attempts to allocate 0 bytes return a pointer to address space which is
 * mmap()ed with PROT_NONE, causing any attempt to use it to fail.
 *
 * The data structures used are arrays and linked lists, which would be
 * unacceptably inefficient for production use but are good enough for
 * testing.
 */

#define PADD(p, c) (void *)((intptr_t)(p) + (size_t)(c))
#define PSUB(p, c) (void *)((intptr_t)(p) + (size_t)(c))
#define PDIFF(p1, p2) (size_t)((char *)(p1) - (char *)(p2))

/* base 2 logarithm of the minimum and maximum block sizes */
#define BUCKET_MIN_SHIFT	4
#define BUCKET_MAX_SHIFT	16

/* bucket size */
#define BUCKET_SIZE		(16*1024*1024)

/* byte values used to fill allocated and unallocated blocks */
#define BUCKET_FILL_ALLOC	0xaa
#define BUCKET_FILL_FREE	0x55

struct bucket {
	void		*base;		/* bottom of bucket */
	void		*top;		/* top of bucket */
	void		*free;		/* first free block */
	void		*unused;	/* first never-used block */
	unsigned long	 nalloc;
	unsigned long	 nfree;
};

struct mapping {
	void		*base;		/* base address */
	void		*top;		/* end address */
	struct mapping	*prev, *next;	/* linked list */
};

/* bucket metadata */
static struct bucket buckets[BUCKET_MAX_SHIFT + 1];

/* mapping metadata */
static struct mapping *mappings;
static unsigned long nmapalloc, nmapfree;

/* if non-zero, all allocations fail */
int t_malloc_fail;

/* if non-zero, all allocations will fail after a countdown */
int t_malloc_fail_after;

/* if non-zero, unintentional allocation failures are fatal */
int t_malloc_fatal;

#if HAVE_UTRACE

/*
 * Record malloc() / realloc() / free() events
 */
static void
trace_malloc_event(const void *o, size_t s, const void *p)
{
	struct { const void *o; size_t s; const void *p; } mu = { o, s, p };
	int serrno = errno;

	(void)utrace(&mu, sizeof mu);
	errno = serrno;
}

#define UTRACE_MALLOC(s, p)						\
	trace_malloc_event(NULL, (s), (p))
#define UTRACE_REALLOC(o, s, p)						\
	trace_malloc_event((o), (s), (p))
#define UTRACE_FREE(o)							\
	trace_malloc_event((o), 0, NULL)

#else

#define UTRACE_MALLOC(s, p)						\
	do { (void)(s); (void)(p); } while (0)
#define UTRACE_REALLOC(o, s, p)						\
	do { (void)(o); (void)(s); (void)(p); } while (0)
#define UTRACE_FREE(o)							\
	do { (void)(o); } while (0)

#endif


/*
 * Return a pointer to inaccessible memory.
 */
static void *
t_malloc_null(void)
{
	struct bucket *b;

	b = &buckets[0];
	if (b->base == NULL) {
		b->base = mmap(NULL, BUCKET_SIZE, PROT_NONE,
		    MAP_ANON | MAP_NOCORE | MAP_NOSYNC | MAP_SHARED, -1, 0);
		if (b->base == MAP_FAILED)
			abort();
		b->top = b->base + BUCKET_SIZE;
		b->free = b->unused = b->base;
	}
	++b->nalloc;
	return (b->base);
}

/*
 * Allocate a direct mapping.  Round up the size to the nearest multiple
 * of 8192, call mmap() with the correct arguments, and verify the result.
 */
static void *
t_malloc_mapped(size_t size)
{
	struct mapping *m;

	if ((m = malloc(sizeof *m)) == NULL)
		return (NULL);
	size = ((size + 8191) >> 13) << 13;
	m->base = mmap(NULL, size, PROT_READ | PROT_WRITE,
	    MAP_ANON | MAP_NOSYNC | MAP_SHARED, -1, 0);
	if (m->base == MAP_FAILED) {
		free(m);
		errno = ENOMEM;
		return (NULL);
	}
	m->top = PADD(m->base, size);
	m->next = mappings;
	m->prev = NULL;
	mappings = m;
	++nmapalloc;
	return (m->base);
}

/*
 * Allocate from a bucket.  Round up the size to the nearest power of two,
 * select the appropriate bucket, and return the first free or unused
 * block.
 */
static void *
t_malloc_bucket(size_t size)
{
	unsigned int shift;
	struct bucket *b;
	void *p;

	/* select bucket */
	for (shift = BUCKET_MIN_SHIFT; (1U << shift) < size; ++shift)
		/* nothing */ ;
	assert(shift >= BUCKET_MIN_SHIFT && shift <= BUCKET_MAX_SHIFT);
	b = &buckets[shift];

	/* initialize bucket if necessary */
	if (b->base == NULL) {
		b->base = mmap(NULL, BUCKET_SIZE, PROT_READ | PROT_WRITE,
		    MAP_ANON | MAP_NOSYNC | MAP_SHARED, -1, 0);
		if (b->base == MAP_FAILED)
			abort();
		b->top = b->base + BUCKET_SIZE;
		b->free = b->unused = b->base;
	}

	/* the bucket is full */
	if (b->free == b->top) {
		errno = ENOMEM;
		return (NULL);
	}

	/* we will return the first free block */
	p = b->free;

	/* update the free block pointer */
	if (b->free == b->unused) {
		/* never been used before, increment free pointer */
		b->free = b->unused = b->unused + (1U << shift);
	} else {
		/* previously used, disconnect from free list */
		b->free = *(char **)p;
		assert(b->free >= b->base && b->free < b->top);
	}

	/* done! */
	++b->nalloc;
	return (p);
}

/*
 * Core malloc() logic: select the correct backend based on the requested
 * allocation size and call it.
 */
void *
t_malloc(size_t size)
{

	/* select and call the right backend */
	if (size == 0)
		return (t_malloc_null());
	else if (size > (1U << BUCKET_MAX_SHIFT))
		return (t_malloc_mapped(size));
	else
		return (t_malloc_bucket(size));
}

/*
 * Allocate an object of the requested size.  According to the standard,
 * the content of the allocated memory is undefined; we fill it with
 * easily recognizable garbage.
 */
void *
malloc(size_t size)
{
	void *p;

	if (t_malloc_fail) {
		errno = ENOMEM;
		return (NULL);
	} else if (t_malloc_fail_after > 0 && --t_malloc_fail_after == 0) {
		t_malloc_fail = 1;
	}
	p = t_malloc(size);
	if (p == NULL && t_malloc_fatal)
		abort();
	memset(p, BUCKET_FILL_ALLOC, size);
	/* XXX fill the slop with garbage */
	UTRACE_MALLOC(size, p);
	return (p);
}

/*
 * Allocate an array of n objects of the requested size and initialize it
 * to zero.
 */
void *
calloc(size_t n, size_t size)
{
	void *p;

	if (t_malloc_fail) {
		errno = ENOMEM;
		return (NULL);
	} else if (t_malloc_fail_after > 0 && --t_malloc_fail_after == 0) {
		t_malloc_fail = 1;
	}
	p = t_malloc(n * size);
	if (p == NULL && t_malloc_fatal)
		abort();
	memset(p, 0, n * size);
	/* XXX fill the slop with garbage */
	UTRACE_MALLOC(size, p);
	return (p);
}

/*
 * Grow or shrink an allocated object, preserving its contents up to the
 * smaller of the object's original and new size.  According to the
 * standard, the object may be either grown or shrunk in place or replaced
 * with a new one.  We always allocate a new object and free the old one.
 */
void *
realloc(void *o, size_t size)
{
	struct mapping *m;
	struct bucket *b;
	void *p;
	size_t osize;
	unsigned int shift;

	/* corner cases */
	if (o == NULL || o == buckets[0].base)
		return (malloc(size));

	/* was this a direct mapping? */
	for (m = mappings; m != NULL; m = m->next) {
		if (o == m->base) {
			/* found our mapping */
			osize = PDIFF(m->top, m->base);
			goto found;
		}
		assert(o < m->base || o >= m->top);
	}

	/* was this a bucket allocation? */
	for (shift = BUCKET_MIN_SHIFT; shift <= BUCKET_MAX_SHIFT; ++shift) {
		b = &buckets[shift];
		if (o >= b->base && o < b->top) {
			/* found our bucket */
			assert(PDIFF(o, b->base) % (1U << shift) == 0);
			osize = 1U << shift;
			goto found;
		}
	}

	/* oops */
	abort();

found:
	if (t_malloc_fail) {
		errno = ENOMEM;
		return (NULL);
	} else if (t_malloc_fail_after > 0 && --t_malloc_fail_after == 0) {
		t_malloc_fail = 1;
	}
	if ((p = t_malloc(size)) == NULL) {
		if (t_malloc_fatal)
			abort();
		return (NULL);
	}
	if (size > osize)
		memcpy(p, o, osize);
	else
		memcpy(p, o, size);
	/* XXX fill the slop with garbage */
	UTRACE_REALLOC(o, size, p);
	free(o);
	return (p);
}

/*
 * Free an allocated object.  According to the standard, the content of
 * the memory previously occupied by the object is undefined.  We fill it
 * with easily recognizable garbage to facilitate debugging use-after-free
 * bugs.
 */
void
free(void *p)
{
	struct mapping *m;
	struct bucket *b;
	unsigned int shift;

	UTRACE_FREE(p);

	/* free(NULL) */
	if (p == NULL)
		return;

	/* was this a zero-size allocation? */
	if (p == buckets[0].base) {
		++buckets[0].nfree;
		return;
	}

	/* was this a direct mapping? */
	for (m = mappings; m != NULL; m = m->next) {
		if (p == m->base) {
			/* found our mapping */
			if (munmap(m->base, PDIFF(m->top, m->base)) != 0)
				abort();
			if (m->prev != NULL)
				m->prev->next = m->next;
			if (m->next != NULL)
				m->next->prev = m->prev;
			if (m == mappings)
				mappings = m->next;
			/* fall through and free metadata */
			p = m;
			++nmapfree;
			break;
		}
		assert(p < m->base || p >= m->top);
	}

	/* was this a bucket allocation? */
	for (shift = BUCKET_MIN_SHIFT; shift <= BUCKET_MAX_SHIFT; ++shift) {
		b = &buckets[shift];
		if (p >= b->base && p < b->top) {
			/* found our bucket */
			assert(PDIFF(p, b->base) % (1U << shift) == 0);
			memset(p, BUCKET_FILL_FREE, 1U << shift);
			/* connect the block to the free list */
			*(char **)p = b->free;
			b->free = p;
			++b->nfree;
			return;
		}
	}

	/* oops */
	abort();
}

/*
 * Print allocator statistics
 */
void
t_malloc_printstats(FILE *f)
{
	struct bucket *b;
	unsigned int shift;

	fprintf(f, "%6s %9s %9s %9s\n", "bucket", "alloc", "free", "leak");
	for (shift = 0; shift <= BUCKET_MAX_SHIFT; ++shift) {
		b = &buckets[shift];
		if (b->nalloc > 0)
			fprintf(f, " 1^%-3u %9lu %9lu %9lu\n",
			    shift, b->nalloc, b->nfree,
			    b->nalloc - b->nfree);
	}
}

/*
 * Test that fails if we leaked memory
 */
static int
t_malloc_leaked(char **desc, void *arg CRYB_UNUSED)
{
	struct bucket *b;
	unsigned int shift;
	unsigned long nleaked;

	for (nleaked = shift = 0; shift <= BUCKET_MAX_SHIFT; ++shift) {
		b = &buckets[shift];
		nleaked += b->nalloc - b->nfree;
	}
	if (nleaked > 0)
		asprintf(desc, "%lu allocation(s) leaked", nleaked);
	else
		asprintf(desc, "%s", "no memory leaked");
	return (nleaked == 0);
}

struct t_test t_memory_leak = {
	.func = &t_malloc_leaked,
	.arg = NULL,
	.desc = "memory leak check",
};
