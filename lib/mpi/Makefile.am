AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcryb-mpi.la

libcryb_mpi_la_SOURCES = \
	cryb_mpi_add.c \
	cryb_mpi_add_abs.c \
	cryb_mpi_cmp.c \
	cryb_mpi_cmp_abs.c \
	cryb_mpi_cmp_i32.c \
	cryb_mpi_cmp_i64.c \
	cryb_mpi_copy.c \
	cryb_mpi_dec.c \
	cryb_mpi_dec_abs.c \
	cryb_mpi_destroy.c \
	cryb_mpi_div.c \
	cryb_mpi_div_abs.c \
	cryb_mpi_eq.c \
	cryb_mpi_eq_abs.c \
	cryb_mpi_eq_i32.c \
	cryb_mpi_eq_i64.c \
	cryb_mpi_gcd_abs.c \
	cryb_mpi_grow.c \
	cryb_mpi_inc.c \
	cryb_mpi_inc_abs.c \
	cryb_mpi_init.c \
	cryb_mpi_load.c \
	cryb_mpi_lsb.c \
	cryb_mpi_lshift.c \
	cryb_mpi_msb.c \
	cryb_mpi_mul.c \
	cryb_mpi_mul_abs.c \
	cryb_mpi_negate.c \
	cryb_mpi_rshift.c \
	cryb_mpi_set.c \
	cryb_mpi_sub.c \
	cryb_mpi_sub_abs.c \
	cryb_mpi_swap.c \
	cryb_mpi_zero.c \
	\
	cryb_mpi.c

noinst_HEADERS = \
	cryb_mpi_impl.h

libcryb_mpi_la_CFLAGS = \
	$(CRYB_CORE_CFLAGS)

libcryb_mpi_la_LIBADD = \
	$(CRYB_CORE_LIBS)

pkgconfig_DATA = cryb-mpi.pc
