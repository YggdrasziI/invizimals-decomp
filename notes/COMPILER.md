# Compiler

Working result (2026-11-24): GCC 4.0.4 with the pspdev Allegrex patch and the
Allegrex tuning of the LBP-PSP-Decomp build kit (build `4.0.4c`), `-O2 -G0`,
reproduces all twelve probe functions. It is an open-source rebuild, not Sony's
`4.0.4+allegrex-2.2.3-psp-2.9.0`; no function is linked from C yet.

## Evidence in the module

| Evidence | Where | Reading |
|---|---|---|
| `GCC: (GNU) 4.0.4+allegrex-2.2.3-psp-2.9.0` | `.comment`, first string | Sony SDK GCC 4.0.4 (allegrex patches 2.2.3, SDK build psp-2.9.0) |
| `GCC: (GNU) 3.3.6+allegrex-2.2.2-psp-1.15.3.internal` | `.comment` | an older SDK GCC, likely in a prebuilt library |
| `GCC: (GNU) 4.0.4+allegrex-2.2.3-psp-2.7.3` | `.comment` | same GCC, older SDK build |
| `GCC: (GNU) 4.0.2 (PSPDEV 20051022)` | `.comment` | homebrew pspdev toolchain, likely a third-party library |
| `.gcc_compiled_long32` | empty section | GCC MIPS marker (`-mlong32`) |
| `.eh_frame`, `.gcc_except_table`, `.jcr`, `.ctors`, `.dtors` | section headers | GCC runtime with C++ exception tables |

The `.comment` section of a linked module keeps one copy of each distinct string, so
the four strings say that four compiler builds contributed objects, not which code
each one built.

## Probes

`tools/probes/gcc_leaf.c`: twelve small functions written back in C (field stores, a
float constant, a 16-byte struct copy, an indexed address, tail calls, a fifth
argument in `$t0`, an inline `sqrt.s`). The only PSP GCC builds of the decomp.me
archive are 32-bit Linux programs, so they run in the manual CI workflow
`.github/workflows/probe.yml` (`tools/psp/compiler_probe.py`, relocated fields masked).

2026-11-24 (runs 45471878294, 45472201118):

| Compiler | Flags | Match |
|---|---|---|
| psp-gcc-1.3.1 (GCC 3.3.3+allegrex-2.2.1-psp-1.3.1) | `-O2 -G0 -ffast-math` | 6/12 |
| psp-gcc-1.3.1 | `-O2 -G0` | 5/12 (no inline `sqrt.s`) |
| psp-gcc-1.3.1 | `-Os -G0` 3/12, `-O1 -G0` 0/12, `-O3 -G0` 0/12 | |
| psp-gcc-1.7.1 (GCC 3.3.3+allegrex-2.2.2-psp-1.7.1, assembler of 1.3.1) | `-O2 -G0 -ffast-math`, with or without `-foptimize-sibling-calls` | 6/12, the same six |

Matching: `func_00018CD0`, `func_000850DC`, `func_0008B5B4`, `func_001203B8`,
`func_001A7D5C`, `func_00008150` (the last needs `-ffast-math` for `sqrt.s`). The six
others differ in three ways that GCC 3.3.3 does not produce whatever the flags:

- tail calls: the game ends `func_0001797C`, `func_0000124C` and `func_00006398` with
  `j` and no stack frame; GCC 3.3.3 saves `$ra` and uses `jal`, even with
  `-foptimize-sibling-calls`;
- multiplication by a constant: `func_00120D34` multiplies by 244 with shifts and
  adds; GCC 3.3.3 uses `mult`;
- block copy: `func_001DA3B0` and `func_001DB5EC` load all four words before storing
  them; GCC 3.3.3 alternates load and store.

All three are what GCC 4.x does on MIPS, which fits the `.comment` strings: GCC 3.3.3
is not the compiler of this code, only close on simple functions.

### GCC 4.0.x rebuilt from GNU sources

Sony's SDK compilers are not available. The build kit of
[Jstar269/LBP-PSP-Decomp](https://github.com/Jstar269/LBP-PSP-Decomp)
(`toolchain/gcc4`, GPL-3.0, commit `754764d1`) builds PSP cross compilers from the
GNU GCC release tarballs (signatures checked) and the ps2dev psptoolchain Allegrex
patch (commit `65aaa94632`, "PSPDEV 20051022"); its `x` / `c` builds add tuning that
project measured on its own retail code (Allegrex in the `cpu` attribute, memory
move and branch costs, an Allegrex scheduling description, `ins` for bit clears, no
`mtc1`/`mfc1` delay, block-copy order and `asm` in delay slots). They assemble with
the Sony binutils 2.14 assembler of the decomp.me `psp-gcc-1.3.1` archive. The manual
workflow `.github/workflows/probe-gcc4.yml` builds them (cached) and runs the probe.

2026-11-24 (run 45472839552, `tools/probes/gcc_leaf.c`):

| Build | `-O2 -G0` | `-Os -G0` | with `-ffast-math` |
|---|---|---|---|
| `4.0.2h` (GCC 4.0.2 PSPDEV 20051022, stock patch) | 11/12 | 5/12 | same |
| `4.0.4h` (GCC 4.0.4, stock patch) | 11/12 | 5/12 | same |
| `4.0.4c` (GCC 4.0.4, patch + LBP tuning) | **12/12** | 6/12 | same |

- `-O2`, not `-Os`: `-Os` changes `func_0000124C`, `func_0008B5B4`, `func_00120D34`,
  `func_001A7D5C` and the block copies.
- `-ffast-math` is not needed: GCC 4.0.x emits `sqrt.s` for `sqrtf` at `-O2`
  (`func_00008150`), unlike GCC 3.3.3.
- The stock builds miss only `func_000850DC`: they store the float before the second
  byte (`sb 0x0`, `swc1 0x4`, `sb 0x1`); the original and `4.0.4c` store both bytes,
  then the float. The difference is instruction scheduling, which the kit's Allegrex
  scheduling description (loads latency 3, stores holding the memory unit) changes.
- The probes cover `.text` from 0x124C to 0x1DB5EC: no region needing another
  compiler has shown up yet. The other `.comment` strings (GCC 3.3.6, pspdev 4.0.2)
  probably come from libraries; to check once the region map exists.

Open: whether `4.0.4c`'s LBP-specific changes (block-copy loop order, `ins`) also
hold for this game: they only matter once a function uses them. A function that
matches with `4.0.4h` but not `4.0.4c` would say so.

Open: block copies of unknown alignment. The original tests the alignment at run
time before a copy loop (`or $v0, <src>, <dst>`, `andi $v0, $v0, 0x3`, `beqz`): one
loop with `lwl`/`lwr` (or `swl`/`swr`), the other with `lw`/`sw`. 16 copies have it:
13 in the game's code (first at 0x2DB48, in `func_0002DACC`), 2 in Bullet (0x152D9C,
in `func_00152B28`) and 1 after it (0x1C7B20). No build of the kit emits the test:
`4.0.4c` gives the unaligned loop only. A function with such a copy cannot match until
the compiler change behind it is found.

Origin (2026-11-24): the test is GCC 3.3's. `gcc/config/mips/mips.c` of GCC 3.3.6,
`expand_block_move`: "If the alignment is not word aligned, generate a test at runtime,
to see whether things wound up aligned, and we can use the faster lw/sw instead
ulw/usw" (`gen_iorsi3 (temp, src_reg, dest_reg)`, `gen_andsi3 (..., UNITS_PER_WORD -
1)`, `beq` to the aligned loop, the unaligned loop first). GCC 3.4.0 and 4.0.4 no
longer have it (`mips_block_move_loop` only). The original's loops are not GCC 3.3's
(there a block move is one insn printed whole, never mixed with the pointer
increments), so the retail GCC 4.0.4 kept 3.3's test in front of 4.0's loop.

Loop orders in the EBOOT (copy loops of 16 bytes per iteration, two pointers):

| Alignment (load/store) | Order | Loops |
|---|---|---|
| aligned / aligned | `sw 0`, both pointers advance, `sw -12/-8/-4` | 416 |
| aligned / unaligned | two words stored, pointers advance, last two at `-8/-4` | 10 (+1 scheduled differently) |
| unaligned / aligned | same | 7 |
| unaligned / unaligned | same | 3 |

Experiment (build `4.0.4a`, outside the kit: `4.0.4c` plus a change of
`mips_block_move_loop`): word-aligned copies as before; otherwise 3.3's run-time test,
then an unaligned loop and an aligned loop, both in the order of the table. With it,
Bullet's `btPersistentManifold::refreshContactPoints` (0x152B28) has the original's size
(342 instructions, 288 with `4.0.4c`) and its copy (0x152D9C) is identical; what is left
differs in the floating-point code before it. The probe object and the build are the
same as with `4.0.4c`. Adopted 2026-11-24 (owner's decision) as the `[compiler]` of
`decomp.toml`: `tools/gcc/block-move-4.0.4.diff` (GPL-3.0-or-later, GCC code), applied
to the kit's `4.0.4c` work tree by `tools/gcc/build_404a.sh` (local and CI).

Scheduling latencies (2026-11-25). The kit's Allegrex scheduling description
(`config/mips/allegrex-sched.md`: loads 3, stores holding the memory unit) leaves the
other instruction types to `generic.md`. The retail compiler's values differ for six
of them. Method: one `cc1plus` per variant of the description, every Bullet file
compiled with the build method below, every function searched in the original
(relocated fields masked); a variant is kept when it adds identical functions and
loses none (`tools/psp/gcc_sched_scan.py` with `library_sweep.py`; the scheduler's
decisions for one function: `tools/psp/gcc_rtl_trace.py`). 994 functions, 443 identical with the kit's description (and `fcvt` 2,
the first value found: `btQuantizedBvh::reportAabbOverlappingNodex`, 0x14B804, has an
independent load between each `trunc.w.s` and its `mfc1`).

| Type | `generic.md` | Retail | Evidence (identical functions) |
|---|---|---|---|
| `fmul`, `fmadd`, single precision | 7 | 4 | alone 443 -> 474; 3: 446 with 2 lost; 5: 447; 6, 8, 9: 443 |
| `fabs`, `fneg`, `fmove` | 2 | 4 | alone 454; 3: 449; 5: 450 with 4 lost; 1: 441 |
| `xfer` (`mfc1`, `mtc1`) | 2 | 3 | alone 448; 1: 437; 4 and 5: 444 with 3 lost |
| `fcmp` | 3 | 4 | no effect alone; with the three above 507 -> 517; 2: 503; 5, 6: 508 |
| `fcvt` | 1 | 4 | on top of the others 517 -> 520; 3: 518; 5: 517 |
| `imul`, `imadd` | 17 | 5 | 505 -> 507 (two `btCapsuleShape` functions); 3, 4: 503; 6, 8, 12: 505 |

All together: **520 identical, none of the 443 lost**. `fadd` stays at 4 (1, 2, 3, 5, 6
all lose functions): every single-precision operation but the division and the
square root takes 4 cycles. Other values tried around the result, all worse or
without effect: load 2 or 4, float loads 2 or 4, any other store reservation (236 to
485), `slt`, `shift`, `arith`, `const`, `call`, `branch` at 2, `fdiv` 4, 12, 15, 40
(23 kept), `fsqrt` 4, 15 (54 kept); `jump`, `unknown`, `multi`, `idiv`, the
double-precision multiplication change nothing. With it
`btBvhTriangleMeshShape::setOptimizedBvh` (0x143EC4; before, `$s0` and `$s1`
exchanged: the first scheduling pass decides the life lengths the local allocator
ranks) and `btPersistentManifold::refreshContactPoints` (0x152B28, 342 instructions)
are identical, and twelve more files have their whole `.text` identical. No probe
function changes (12/12). Adopted in build `4.0.4a`:
`tools/gcc/allegrex-sched-4.0.4.diff`, applied by `tools/gcc/build_404a.sh` after the
block-copy diff. Scheduler options (`-fno-sched-*`, `-fsched2-use-*`) do not help
the near misses that remain.

Rescan of 2026-11-25, with the branch cost and host `qsort` below (546 identical):
every adopted value is still the best (any other value of `fadd`, single `fmul`,
the moves, `fcmp`, `fcvt`, `xfer`, `imul`, the loads split by kind, `arith`,
`const`, `slt`, `shift`, `call` loses functions). Open: a single-precision
`fsqrt` of 18 to 24 (generic: 54) makes `btGjkPairDetector::getClosestPoints`
(0x151954, 735 instructions) identical, but changes the order around `sqrt.s` in
three inline functions of `btGjkEpa2.cpp` (0x1DE2C4, 0x1DE9B4, 0x1DF248) that are
identical with 54; 25 and more keep those and lose the first. No single latency
fits both: not adopted. `library_sweep.py --inline 1BA6D0 1E3294` scores the inline
functions too (1166 identical of 1695 with them, 547 of 994 in `.text` alone): use
it for any compiler change.

**Branch cost** (2026-11-25). `BRANCH_COST` (`config/mips/mips.h`) is 2 for the
Allegrex in the kit; the retail compiler behaves as with 3. The if-conversion pass
moves a block before the branch that skips it when the block costs less than
`BRANCH_COST` instructions (`ifcvt.c`, `find_if_case_1` / `find_if_case_2`), so with 3
a block of two register loads goes before the branch. Proof: `btBoxShape::getEdge`
(0x1D95FC, an inline function of `btBoxShape.h`) loads `edgeVert0 = 0` and
`edgeVert1 = 0` in the entry block, before the `beqz` of the `switch` bounds test;
with 2 they stay in a block after the jump. No flag gives it (`-fno-tree-ccp
-fno-tree-dominator-opts` comes within two instructions and loses 243 functions).
On the Bullet sweep, 2: 520 identical of 994; 3: 528, none lost; 4: 525, two lost.
The eight gained include the near misses that scheduling did not explain:
`btConvexConvexAlgorithm::processCollision` and `calculateTimeOfImpact`,
`btQuantizedBvh::updateSubtreeHeaders` and `deSerializeInPlace`. Adopted in build
`4.0.4a`: `tools/gcc/branch-cost-4.0.4.diff`. Not explored: values per optimisation
level. The arithmetic costs of `mips_rtx_costs` (float addition 2-4, single
multiplication 4-5, double 5, division 12 and 30, integer division 35) change
nothing on Bullet; an integer multiplication below 5 loses four functions.

**Host qsort** (2026-11-25). GCC 4.0 sorts the stack variables of a function by size
with the host's `qsort` and no tie-break (`cfgexpand.c`, `partition_stack_vars`,
`stack_var_size_cmp`), so the stack slots of equal-sized variables depend on the C
library the compiler was built with. glibc's `qsort` is a stable merge sort; the
retail compiler behaves as built with the BSD `qsort` (the one of newlib, so of a
Cygwin host: hypothesis, only the behaviour is proven), an insertion sort below 7
elements and a quicksort that moves equal elements from 7 on. Symptom: a function
with many temporaries of one size (`btVector3`) is identical but for stack offsets
exchanged between two of them (`btManifoldResult::addContactPoint`, 0x141748: slots
0x30-0x3C and 0x0-0xC). With a BSD `qsort` in `cc1` / `cc1plus`, the Bullet sweep
goes from 528 to 546 identical functions, none lost (the variant without the
switch to insertion sort when no swap happened gives the same). Adopted in build
`4.0.4a`: `tools/gcc/host-qsort-4.0.4.diff` (appended to `config/mips/mips.c`).
Every other `qsort` call of GCC gets it too.

**madd** (2026-11-25). The game has 135 `madd` / `msub` (98 in the game code below
0x133CCC, 28 in Bullet, 22 elsewhere); the kit's compiler emits
none. `ISA_HAS_MADD_MSUB` (`config/mips/mips.h`) leaves the Allegrex out, so the
`*mul_acc_si` / `*mul_sub_si` patterns of `mips.md` are off; the kit's own
`allegrex_madd` ties the accumulator to the result and combine never forms it.
Evidence: `btStridingMeshInterface::InternalProcessAllTriangles` (0x14FD44) computes
`base + index * stride` as `mtlo base` / `madd index, stride` / `mflo`, where the kit
gives `mult` / `mflo` / `addu`. With the Allegrex in `ISA_HAS_MADD_MSUB` alone, combine
forms `*mul_acc_si` but reload takes its third alternative (accumulator and result in
general registers, no reload needed), whose splitter wants a three-operand `mul` the
Allegrex lacks (internal compiler error); with that splitter replaced by `mult` /
`mflo` / `addu`, no `madd` at all and no change on Bullet. Without the third
alternative (in `*mul_sub_si` too), the accumulator goes through LO and the game's
sequence comes out: Bullet sweep 1166 -> 1168 of 1695 (with the inline functions),
none lost (`btRandInt2`, the `processNode` of `btBvhTriangleMeshShape::
processAllTriangles`); `btOptimizedBvh::updateBvhNodes` goes from 20 % to 94 %,
`performRaycast` / `performConvexcast` from 47 % to 75 %, `InternalProcessAllTriangles`
from 56 % to 70 % (left: order of `mtlo` and loads, registers). The exact retail
pattern is not known, only this behaviour; `btCollisionDispatcher::
freeCollisionAlgorithm` (0x13BD1C) has the `madd` but another register allocation
(the argument copied to `$a3`). Adopted in build `4.0.4a`: `tools/gcc/madd-4.0.4.diff`.

**GCSE hash order** (2026-11-25), not adopted. In `btPolyhedralConvexShape::getAabb`
(0x14E9D0) the only difference is that the loads at `trans+16` and `trans+32` get
`$a0` / `$a1` swapped. Their pseudos are made by GCSE PRE (`pre_delete`, `gcse.c`),
which walks the expression hash table bucket by bucket; global allocation then breaks
the tie by allocno number, so the first one made gets `$a0`. Ours makes `+16` first
(bucket 2 of 37, `+32` in bucket 18: `hash_rtx` puts them 16 apart whatever the
table), the game `+32`. Tried on all of Bullet (with the inline functions, base
1168/1695):

- table size (`n_insns / 4`, made odd) changed to `/ 2`: +3 -2; `/ 3`: +2 -2; `/ 5`:
  +2 -0; `/ 6`: +4 -2; `/ 8`: +2 -2; `/ 4` not made odd: +4 -0;
- one size forced for every function, 11 to 260: 17 functions depend on it; each
  of the family (a) functions has the game's code for some sizes (the getAabb above
  for 39 of the 250), but no formula of `n_insns` gives all of them (best 11 of 13
  with a known insn count; the default 4);
- a constant added to every hash, 0 to 399: best +7 -8 (both getAabb,
  `btCompoundShape::getAabb`, `btPoint2PointConstraint::buildJacobian`,
  `resolveSingleBilateral`, `btOptimizedBvh::refitPartial`, a `NodeTriangleCallback`,
  for three `btContactConstraint` solvers, three `btGjkEpa2` functions,
  `btOptimizedBvh::build` and `btPoint2PointConstraint::solveConstraint`).

So the bucket order decides these functions, but the retail hash values differ
expression by expression, not by a table size or an offset. Hypothesis: other
register numbers (`hash_rtx` adds the register number of a `REG`), that is, another
pseudo numbering before GCSE. The `/ 4` not odd variant (+4 -0) is a fit, not
evidence, and is not adopted.

**Host word size** (2026-11-25): no effect. `HOST_WIDE_INT` is 64 bits on any host
for a MIPS target (`need_64bit_hwint=yes` in `config.gcc`), and `cc1` / `cc1plus`
built as 32-bit programs (i686 Linux, static) give the same 546 identical Bullet
functions, none gained or lost.

### Adopted

2026-11-24: `4.0.4c` at `-O2 -G0` is the `[compiler]` of `decomp.toml`. Built in WSL
(Ubuntu 24.04) with the same recipe, its code generation fingerprint equals the kit's
(`check.sh`: `FINGERPRINT_OK`; the binaries differ only by the embedded install path)
and the probe gives 12/12 locally. The CI build job builds it on a cache miss, checks
the fingerprint and runs the probe on every push.

### Stack alignment: `-mpreferred-stack-boundary=4`

2026-11-24. Every stack frame of the game (2831 functions below 0x133CCC), of Bullet,
of the libraries and of the tail is a multiple of 16 bytes, and the floating-point
save area starts on a 16-byte boundary (`swc1 $f20, 0x10($sp)` after `$ra` and `$s0`
at 0x4 and 0x0). The kit's builds default to 8 bytes (EABI), so `4.0.4c` gives a
0x10-byte frame with `$f20` at 0x8 for the same function. `-mpreferred-stack-boundary=4`
(an option of these builds) gives the original layout; the twelve probes stay
identical with it. expat's frames are 8-aligned (59 of 110): the stock PSPDEV build
there keeps its default.

## expat 2.0.0: the stock PSPDEV GCC 4.0.2

2026-11-24. The upstream sources of expat 2.0.0 (`src/expat`, unmodified), compiled at
`-O2 -G0` and searched by content in 0x17661C-0x18DE70 (relocated fields masked):

| Build | `xmlrole.c` | `xmltok.c` |
|---|---|---|
| `4.0.4c` (the game's) | 35/48 | 30/102 |
| `4.0.4h` (stock patch) | **48/48** | 97/102 |
| `4.0.2h` (PSPDEV 20051022) | **48/48** | 97/102 |

- So the library was not built with the game's compiler: the LBP tuning of `4.0.4c`
  (scheduling) changes it. `4.0.2h` and `4.0.4h` give the same code here; `4.0.2h` is
  the one the module names (`.comment` `GCC: (GNU) 4.0.2 (PSPDEV 20051022)`), so it is
  the `[compiler.dirs."src/expat"]` of `decomp.toml` (hypothesis between the two until
  a function tells them apart). Its fingerprint equals the kit's (`check.sh 4.0.2h`:
  `FINGERPRINT_OK`).
- `-O2`: `-O3`, `-Os` and `-O2 -fno-schedule-insns` lose most functions.
- `XML_DTD` and `XML_NS` (both defined by expat's own configure script) are off: with
  them `xmlrole.c` and `xmltok.c` hold functions the original does not have, and
  without them the whole `.text` of `xmlrole.c` (0x19D8 bytes) is identical at
  0x1805B8, with its `.rodata` at 0x20E624.
- `assert` calls a function of the game, `func_0000FE5C(file, line, expression)`
  (`xmlparse.c`, line 1744 at 0x178FE8): `NDEBUG` is off; the matching `assert.h` is
  still to write.
- Left: `xmltok.c` (`normal_scanPi`, `little2_sameName`, three `nameMatchesAscii`) and
  `xmlparse.c` (`XML_ParserFree`, `doContent`) differ by block order and register
  allocation; flags tried without gain: `-fno-reorder-blocks`, `-fno-crossjumping`,
  `-fno-gcse`, `-fno-tree-pre`, `-fno-loop-optimize`, `-fno-strict-aliasing`,
  `-fno-unit-at-a-time`, `-fno-tree-loop-optimize` (with `4.0.4h`).

## Bullet: how its files were built

2026-11-25, from whole files that match (`notes/SPLITS.md`, "Bullet files"):

- the game's compiler (`4.0.4a`) at `-O3 -G0 -mpreferred-stack-boundary=4 -DNDEBUG`
  (`-O3` gives the same code as `-O2 -finline-functions`; `NDEBUG`: no `assert`
  strings);
- run from the Bullet root with `-I.`: the `__FILE__` strings are
  `BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp` and
  `./LinearMath/btAlignedAllocator.h` (header found through `-I.`); `decomp.toml` runs
  the compiler in `src/bullet` (`tools/psp/compile.py --cwd`);
- constants and strings in their mergeable sections (`.rodata.cst4`,
  `.rodata.str1.4`, the default `-fmerge-constants`): each file's floats sit together
  in `.rodata` and its strings with the other files' strings (0x2379F4-0x237F64),
  and the link merged none of them (`./LinearMath/btAlignedAllocator.h` is there once
  per file, and a file's `.rodata.cst4` keeps its duplicates, 0.5, 1.0, 0.5, 1.0 for
  `btCollisionObject.cpp`). GNU ld would merge them: the build drops their merge flag
  (`compile.py --rename-section` with explicit flags);
- `operator delete(void *)` declared `throw()` as the standard does: a deleting
  destructor that only calls it is then nothrow, and GCC 4.0 writes no `.eh_frame`
  entry for it, as in the original (`include/cxx/new`);
- one `.eh_frame` CIE per object, then its FDEs, not merged; `.gnu.linkonce.*`
  sections kept from the first file of the link (`btEmptyCollisionAlgorithm.cpp`'s
  copy of `btCollisionAlgorithm`'s vtable and destructors, before
  `btCollisionAlgorithm.cpp`), GNU ld's rule for them too;
- an `.eh_frame` entry that describes a discarded `.gnu.linkonce.t` copy keeps its
  place and holds the address of the copy the link kept (GNU ld 2.4x writes 0
  there): the build points these relocations at the function's symbol
  (`tools/psp/compile.py --linkonce-symbols`).
