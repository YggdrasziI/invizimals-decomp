# Provenance of reused code and data

Every file, function or data set taken from another project, with the project, its
commit, its license (or its absence and the owner's approval) and the changes made.

## Tools (`tools/psp`, `tools/common`)

Shared tools come from two submodules of the owner, both CC0-1.0, which record their
own provenance (their `README.md`):

- `tools/psp`: [psp-decomp-tools](https://github.com/YggdrasziI/psp-decomp-tools);
- `tools/common`: [decomp-tools](https://github.com/YggdrasziI/decomp-tools).

`tools/configure.py` is the owner's jd-tlf-decomp `configure.py` (CC0-1.0), with the
compiler made optional; `tools/extract.py` follows the owner's mhfu-decomp
`extract.py` (CC0-1.0) and adds the decryption. `LICENSE` (CC0-1.0) is the same as
the sibling projects'.

## pspdecrypt

[John-K/pspdecrypt](https://github.com/John-K/pspdecrypt) release 1.0 (GPL-3.0;
`pspdecrypt-1.0-windows.zip` SHA-1 `8f5f3dbf9e82ce39f6a4e6e60e881ce989961fc5`) is
downloaded into `tools/pspdecrypt/` (git-ignored) and run as a program by
`tools/extract.py`. None of its code is in this repository.

## NID names (`tools/psp/nids.txt`)

Facts NID -> function name extracted from PPSSPP (`Core/HLE/*.cpp`,
GPL-2.0-or-later), no code; see `tools/psp/README.md`.

## Project documents

`SPEC.md` and `notes/` follow the structure of the owner's jd-tlf-decomp and
mhfu-decomp projects; every fact about this game was established again here.

## expat 2.0.0 (`src/expat`)

The game links expat 2.0.0 (version string `expat_2.0.0` at 0x179384). `src/expat`
holds the `lib/*.c`, `lib/*.h` and `COPYING` of the release tarball
`expat-2.0.0.tar.gz` (SourceForge, SHA-256
`c4280eb80fa8e14b3f204b1d0ec67af66fa04c4f9dae9c78a1a8ee4c6f485ddd`; the same files as
the GitHub tag `R_2_0_0` of libexpat), **unmodified**. License: MIT (`src/expat/COPYING`).
Written for this project: `include/expat/expat_config.h` (the build configuration,
in place of the configure script's output) and `include/libc/` (minimal declarations).

## Bullet Physics, trunk 115018330038 (`src/bullet`)

The game links Bullet Physics from its trunk between the 2.70 and 2.71 releases
(`notes/SPLITS.md`, `notes/COMPILER.md`), from between two commits:

- after `4894f24c9a2b` (2008-08-01, "Fixes for FPU exceptions"): the game's
  `btOptimizedBvh::walkStacklessQuantizedTreeAgainstRay` (0x14AED0) tests each
  component of the ray direction against zero before inverting it, as that commit
  added;
- before `7838a5c98088` (2008-08-07), which added the virtual
  `btDiscreteDynamicsWorld::setNumTasks`: the game's vtable of that class (0x243048)
  has 27 functions and ends before it. The next commit, `551f0fa1ec68` (raw pointers
  kept by `btAxisSweep3` and `btSimpleBroadphase`), loses 16 identical functions.

The commits in between change only build files and demos, so the library sources are
those of `115018330038` (2008-08-02), the last commit before `7838a5c98088`, of
[bulletphysics/bullet3](https://github.com/bulletphysics/bullet3). Function by
function it gives 16 identical functions more than the trees from `551f0fa1ec68`
to the 2.71 commit `66861ad3340a`, and loses none the game has. `src/bullet` holds the `*.cpp` and `*.h` of `src/LinearMath`,
`src/BulletCollision` and `src/BulletDynamics`, the three headers of `src/`
(`btBulletCollisionCommon.h`, `btBulletDynamicsCommon.h`, `Bullet-C-Api.h`) and
`BulletLicense.txt` of the GitHub archive of that commit
(`codeload.github.com/bulletphysics/bullet3/tar.gz/115018330038`, SHA-256
`686584be797d19c3e2d8595eadb7ebfc399b7bd6ddc18e4da6d5501d76f7a23b`). License: zlib
(`src/bullet/BulletLicense.txt`); line endings normalized to LF. Files changed for the
game's build are marked as such at their top (zlib license, clause 2) and listed here;
each change is a reconstruction from the game's code, not a copy of other code:

- `LinearMath/btQuadWord.h`: `btQuadWordStorage` declared `ATTRIBUTE_ALIGNED16` (the
  upstream declaration, commented out there): the game's vectors are 16-byte aligned;
- `LinearMath/btScalar.h`: `ATTRIBUTE_ALIGNED16` aligns to 16 bytes for this target;
- `LinearMath/btVector3.h`: `btVector3::cross` as the game's VFPU block (79 times in
  the Bullet code, first at 0x152738);
- `LinearMath/btMatrix3x3.h`: `setRotation` as the game's VFPU block (first at
  0x13FCC8; no division by the squared length);
- `LinearMath/btQuaternion.h`: `operator*=` and `operator*(q1, q2)` with `vqmul.q`
  (first at 0x157F00).
- `LinearMath/btAlignedAllocator.h`: `btAlignedAlloc` and `btAlignedFree` call the
  game's allocator (0xBDE8 with alignment, size and the calling file's name, 0xBE04),
  as every Bullet function of the game does.
- `BulletCollision/CollisionShapes/btOptimizedBvh.h`: `btQuantizedBvhNode`
  without `ATTRIBUTE_ALIGNED16` (the upstream declaration, commented out there):
  `btOptimizedBvh::build` (0x14E194) keeps a temporary of that type at `sp+0x14`,
  which a 16-byte aligned type cannot have; with the change the function is
  identical and no other function of the library changes.

Written for this project: `include/cxx/` (minimal C and C++ declarations for Bullet;
`new` declares `std::exception`, `std::bad_alloc` and the global allocation functions
with the standard's exception specifications).

## GCC 4.0.x build kit

The probe workflow `.github/workflows/probe-gcc4.yml` fetches `toolchain/gcc4` of
[Jstar269/LBP-PSP-Decomp](https://github.com/Jstar269/LBP-PSP-Decomp) at commit
`754764d14759c58969bfe29cdc9ab5346255b7ee` (kit files GPL-3.0-or-later, GCC and the
pspdev patches GPL-2.0-or-later) and runs its `build.sh` in CI (builds `4.0.4c` and
`4.0.2h`, the latter for `src/expat`). Nothing of the kit is
copied into this repository. It also downloads the pspdev release `v20261001` (for
`psp-ld`) and the decomp.me `psp-gcc-1.3.1` archive (for the Sony assembler).

## GCC change (`tools/gcc/block-move-4.0.4.diff`)

A diff of `gcc/config/mips/mips.c` of GCC 4.0.4 (GPL-2.0-or-later), written for this
project and marked GPL-3.0-or-later: its context holds the LBP-PSP-Decomp kit's change
C1 (`lbp-copy-order-asm-slot-4.0.4.diff`, GPL-3.0-or-later,
[Jstar269/LBP-PSP-Decomp](https://github.com/Jstar269/LBP-PSP-Decomp) commit
`754764d14759`), and its run-time alignment test follows GCC 3.3.6's
`expand_block_move` (GPL-2.0-or-later). Neither GCC nor the kit is copied into the
repository; `tools/gcc/build_404a.sh` applies the diff to the kit's work tree.

## GCC change (`tools/gcc/allegrex-sched-4.0.4.diff`)

A diff of `gcc/config/mips/allegrex-sched.md`, a file the LBP-PSP-Decomp kit copies
into GCC 4.0.4 (`toolchain/gcc4/patches/allegrex-sched.md`, GPL-3.0-or-later, commit
`754764d14759`): written for this project, marked GPL-3.0-or-later, it appends six
reservations (instruction latencies). The kit is not copied into the repository; `tools/gcc/build_404a.sh` applies the diff to the
kit's work tree.

## GCC change (`tools/gcc/branch-cost-4.0.4.diff`)

A diff of `gcc/config/mips/mips.h` of GCC 4.0.4 (GPL-2.0-or-later) as the
LBP-PSP-Decomp kit patches it (GPL-3.0-or-later, commit `754764d14759`): written for
this project, marked GPL-3.0-or-later, it changes one macro (`BRANCH_COST` for the
Allegrex). Neither GCC nor the kit is copied into the repository;
`tools/gcc/build_404a.sh` applies the diff to the kit's work tree.

## GCC change (`tools/gcc/host-qsort-4.0.4.diff`)

A diff of `gcc/config/mips/mips.c` of GCC 4.0.4 as the LBP-PSP-Decomp kit patches it:
it appends a `qsort`, the 4.4BSD algorithm (Bentley and McIlroy; the one of newlib's
`libc/search/qsort.c`) rewritten for this project, BSD-3-Clause, copyright The
Regents of the University of California, notice kept in the code. Neither GCC nor
the kit is copied into the repository; `tools/gcc/build_404a.sh` applies the diff to
the kit's work tree.

## GCC change (`tools/gcc/hard-regs-4.0.4.diff`)

A diff of `gcc/config/mips/mips.h` and `mips.c` of GCC 4.0.4 (GPL-2.0-or-later) as the
LBP-PSP-Decomp kit patches them (GPL-3.0-or-later, commit `754764d14759`): written for
this project, marked GPL-3.0-or-later, it appends 131 fixed registers to the register
tables. Neither GCC nor the kit is copied into the repository;
`tools/gcc/build_404a.sh` applies the diff to the kit's work tree.

## GCC change (`tools/gcc/insv-4.0.4.diff`)

A diff of `gcc/config/mips/mips.md` of GCC 4.0.4 as the pspdev patch and the
LBP-PSP-Decomp kit change it (GPL-2.0-or-later and GPL-3.0-or-later, kit commit
`754764d14759`): written for this project, marked GPL-3.0-or-later, it changes one
operand predicate of the `insv` expander. Neither GCC nor the kit is copied into the
repository; `tools/gcc/build_404a.sh` applies the diff to the kit's work tree.
