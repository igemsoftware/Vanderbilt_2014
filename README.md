darwin
======

Track changes as a genome evolves over time (hence the name).

#### Build

To build, run 'scons' in the root directory; the executable will be placed there when compilation finishes. Run 'scons --clean' to remove all build files.

Currently does not build on Windows; this situation will soon be rectified.

#### Dependencies

* scons
* GLib
* gmp
* a C compiler

#### Description

A full-featured version-control system (VCS) for biological parts. Formats standard genetic files (fasta, genbank, ape, etc) to be processed extremely efficiently by a standard VCS (git, svn, hg, bzr, cvs, etc) and throws a CLI/GUI layer on top of it so an unskilled user doesn't need to worry about the underlying abstraction.

...or at least, that's the idea. We're getting there; watch or star this repository for updates.
