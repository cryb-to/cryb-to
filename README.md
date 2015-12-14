# The Cryb libraries

The Cryb libraries are a collection of cryptography- and
security-related function libraries written with the following goals
in mind:

* Comprehensive: Cryb aims to provide a rich and flexible set of
  building blocks for cryptographic applications.

* Self-contained and easily embeddable: the Cryb libraries have no
  external dependencies (apart from the toolchain) and few internal
  ones.  Individual modules and algorithms can easily be extracted
  from Cryb and integrated into other codebases.

* Reliable: the libraries come with an extensive test suite with a
  long-term goal of 100% test coverage.

* Stable: guaranteed API and ABI stability.

* Documented: full API documentation in the form of Unix man pages.

* Consistently and permissively licensed: the entire collection is
  under the 3-clause BSD license.

## Caveat

We aren't there yet.  The Cryb libraries are still undeniably at the
experimental stage.  However, significant portions are already in
production use as components in other projects (cf: easily
embeddable), and we have great expectations for the future of Cryb.
