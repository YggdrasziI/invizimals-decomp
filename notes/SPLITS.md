# Splits

Bounds of the splat pieces of `config/UCES01241/splat.eboot.yaml` and what proves
them. A split that builds proves the bytes, not a translation unit.

## Current pieces (2026-11-24)

| File offset | vram | Piece | Contents |
|---|---|---|---|
| 0x0 | | `bin elf_header` | ELF header, program headers |
| 0xC0 | 0x0 | `asm text` | `.init`, `.text` from 0x30 |
| 0x18D90 | 0x18CD0 | `c unit_00018CD0` | `func_00018CD0` only; bounds are the function's own (hypothesis: the translation unit is not known) |
| 0x18DB0 | 0x18CF0 | `asm text_00018CF0` | `.text` to 0x1805B8 |
| 0x180678 | 0x1805B8 | `c expat/xmlrole` | expat 2.0.0 `xmlrole.c`, whole file (below) |
| 0x182050 | 0x181F90 | `asm text_00181F90` | rest of `.text` (to 0x1E3294), `.fini` |
| 0x1E3370 | 0x1E32B0 | `asm scestub` | `.sceStub.text` |
| 0x1E3E10 | 0x1E3D50 | `rodata lib` | `.lib.ent*`, `.lib.stub*`, `.rodata.sceModuleInfo` |
| 0x1E41AC | 0x1E40EC | `rodata sceresident` | `.rodata.sceResident` |
| 0x1E44B4 | 0x1E43F4 | `rodata scenid` | `.rodata.sceNid` |
| 0x1E4A04 | 0x1E4944 | `rodata ctors` | `.ctors`, `.dtors`, `.jcr` |
| 0x1E4BDC | 0x1E4B1C | `rodata eh_frame` | `.eh_frame`, `.gcc_except_table` |
| 0x20B840 | 0x20B780 | `rodata rodata` | `.rodata`, `.rodata.0001`-`.0106`, padding, to 0x20E624 |
| 0x20E6E4 | 0x20E624 | `.rodata expat/xmlrole` | `xmlrole.c`'s `.rodata` (keywords, jump tables) |
| 0x20EA08 | 0x20E948 | `rodata rodata_0020E948` | rest of the read-only data |
| 0x243840 | 0x243780 | `data data` | `.data`, `.data.0001` |
| (bss) | 0x248198 | `bss bss` | `.sbss`, `.bss`, `.bss.0001`, `.bss.0002` (0x40B4D8) |
| 0x248258 | | `bin elf_tail` | relocation tables, `.comment`, `.shstrtab`, section headers |

## Region map (UCES01241, first pass 2026-11-24)

Evidence used: strings of `.rodata` and the functions that load them (`%lo` operands
of the splat assembly), calls to the import stubs (`symbol_addrs.eboot.txt`), and
whether a function has an `.eh_frame` FDE (3454 of 7829 functions do: code compiled
with unwind tables, C++ here; C libraries and C++ built without exceptions have
none). A bound is placed on the first function where the evidence changes (start or
end of a run without FDEs, first identifying string); it is **approximate** until the
units on each side match. Library names come from strings of the binary; versions
only where a string gives one.

| Range (vram) | Region | Evidence |
|---|---|---|
| 0x30-0x51C | Start-up (module start, main thread) | no FDE; `0xB4` creates the `user_main` thread, `0x228` calls `sceKernelExitThread` / `sceKernelGetModuleIdByAddress` |
| 0x51C-0x133CCC | Game (Novarama) | C++ (FDEs), strings of the game's classes (`CMemMngr`, `CResourcesMngr`, `TCapture*`, `TVideo`), its asset paths; ends with the MPEG/JPEG wrappers (`sceMpeg*` messages, 0x131C64-0x133B58) |
| 0x133CCC-0x139438 | C++ demangler (libiberty `cp-demangle`, from libsupc++: hypothesis) | 39 functions without FDE using `_GLOBAL_`, `string literal`, `reference temporary for `, ` restrict` |
| 0x139438-0x13A2C0 | unidentified | small C++ functions (FDEs), no strings |
| 0x13A2C0-0x17864C | Bullet Physics | source paths `BulletCollision/...cpp`, `BulletDynamics/...cpp`, `./LinearMath/btAlignedAllocator.h` referenced in address order from 0x13A2C0 to 0x174200; the floating-point functions 0x176634-0x17864C are Bullet's `btContactConstraint.cpp` (`resolveSingleBilateral`, `resolveSingleCollision*`, `resolveSingleFriction*`, now linked from source); end: the start of `xmlparse.c` |
| 0x17864C-0x18DE70 | expat 2.0.0 (C) | no FDEs; `expat_2.0.0` (0x179384), `xmlparse.c` assertion (0x178FE8), XML namespace URIs and DTD keywords (0x1797B8-0x18DA8C); start: the first function of `xmlparse.c` (by content); end: the only alignment padding of `.text` (3 nops at 0x18DE64) |
| 0x18DE70-0x18E430 | inflate routine, hand-written assembly (SDK deflate library: hypothesis) | 16-aligned after the padding; one routine that splat splits at its inner entries (`rotrv`, temporaries only, 0x380-byte frame); its only caller, 0x1AC080, checks a zlib header (`CMF & 0xF == 8`, `FDICT`) and returns SDK-style error codes (`0x80000004`) |
| 0x18E430-0x18FE78 | libgcc soft-float double (`fp-bit` / `dp-bit`: hypothesis) | no FDEs; arguments packed on the stack and unpacked by shared helpers; 0x18E6B4 has 600 callers (float to double, as every `printf` of a float needs), 0x18EA3C / 0x18EAB4 / 0x18EB20 have 125 / 130 / 188 (add, sub, mul), then seven functions of one shape (the seven comparisons); called mostly from the C library |
| 0x18FE78-0x194B44 | unidentified runtime | small accessors (offsets 0x4D0-0x4E0) called from the C library, then mixed FDE / no FDE code masking interrupts (`sceKernelCpuSuspendIntr` / `ResumeIntr`) |
| 0x194B44-0x1952A0 | C library system glue (SDK libc: hypothesis) | `_exit`, `UserSbrk`, wrappers of `sceIo*` with `sceKernelStd*`, `sceKernelLibcClock` / `Time` / `Gettimeofday`; message `libc:%s: should be called from main thread` |
| 0x1952A0-0x199B9C | unidentified | |
| 0x199B9C-0x19D5E8 | libgu (SDK graphics library: hypothesis) | `SceGuSignal`, `sceGe*`, `sceDisplaySetFrameBuf` calls |
| 0x19DC20-0x1A2690 | math library | function name strings `fmod`, `acosf`, `asinf`, `atan2f`, `powf`, `sqrtf` (error reporting) |
| 0x1A627C-0x1A7D40 | libstdc++ / libsupc++ runtime | `gthread_lock` mutex, `terminate called after throwing an instance of '` |
| 0x1A7DAC-0x1A9620 | libwave (SDK: hypothesis) | `SceWaveMainEvf`, `sceAudioCh*`, `sceAudioOutput*` |
| 0x1AC2A0-0x1ACE24 | libsas wrappers (SDK: hypothesis) | thin wrappers of every `__sceSas*` import |
| 0x1AF818-0x1BA6D0 | C library (reentrancy, stdio, locale, number formatting) | `_getmodreent`, `libc:%s: no reent structure`, `C-UTF-8`, `Infinity`, `e%ld` |
| 0x1BA6D0-0x1E3294 | inline functions and template instances (`.gnu.linkonce.t.*` placed after the rest: hypothesis) | tiny game getters returning names (`menu_theme`, capture names in near-alphabetical class order), Bullet `btAlignedObjectArray` templates (`./LinearMath/btAlignedAllocator.h`), SDK calls of the game (`sceNpRoster*`, `sceG729*`) |

### expat 2.0.0 files

The upstream sources compiled with the stock PSPDEV GCC 4.0.2 at `-O2 -G0`, without
`XML_DTD` and `XML_NS` (`notes/COMPILER.md`), found by content in the original
(relocated fields masked). The files sit in alphabetical order.

| Range | File | Status |
|---|---|---|
| 0x17864C-0x1805B8 | `xmlparse.c` | 114/116 functions identical; `XML_ParserFree` (register allocation in the tag list loop) and `doContent` (93.8 %) differ |
| 0x1805B8-0x181F90 | `xmlrole.c` | identical as a whole file (`.text` 0x19D8 bytes, `.rodata` 0x324 bytes at 0x20E624); linked from source |
| 0x181F90-0x18DE70 | `xmltok.c` | 97/102 functions identical; `normal_scanPi`, `little2_sameName` and the three `nameMatchesAscii` differ (block order, registers) |

### Bullet files

Trunk 110e8313dab1 (between 2.70 and 2.71) with the game's changes (`notes/PROVENANCE.md`),
built as `notes/COMPILER.md` ("Bullet: how its files were built") describes, found by
content in the original. A whole C++ file is linked with its pieces at their places:
`.text`; its floats (`.rodata.cst4`) and strings (`.rodata.str1.4`, 0x237xxx); its
out-of-line inline functions (`.gnu.linkonce.t.*`, in 0x1BA6D0+); its typeinfo, names
and vtables (`.gnu.linkonce.r.*`, 0x2414xx-0x2436xx); its `.eh_frame` block (one CIE
per object, then its FDEs) and its `.gcc_except_table`. A `.gnu.linkonce` section
shared by several files is linked from the first one. A later file's copy of such a function can
be identical and still fail to place: it refers to its own file's constants or
strings, and the kept copy to the first file's (`btCylinderShape.cpp`'s copy of
`btBoxShape::getPreferredPenetrationDirection` loads from its own `.rodata.cst4`, the
kept one at 0x1D9724 from `btBoxShape.cpp`'s at 0x215BD0). Such a section is the
discarded duplicate; it does not keep the file from being linked. Two classes whose
functions all look identical are not in the game: `btOdeJoint` and `btEmptyShape`
(their typeinfo names `10btOdeJoint` / `12btEmptyShape` and the string `Empty` of
`getName` are absent; their few-instruction constructors and destructors match other
classes'). The game's `btOptimizedBvh.cpp` (0x147468-0x14E870) is the one from before
Bullet commit `79b6f7a77219` (2008-08-26, which moved most of it into
`BroadphaseCollision/btQuantizedBvh.cpp`): the typeinfo name `14btQuantizedBvh` is
absent, `btOptimizedBvh`'s typeinfo (0x2426B8) has no base class, its vtable (0x2426DC)
points into this range (D1 0x148830, D0 0x1484A4), and the file of commit
`8eafab367fc2` (the same as `110e8313dab1`'s, with the two game changes) gives every
function at the game's offset from 0x147468 up to `updateBvhNodes` (0x14C96C), the
only one not identical (93.6 %: the last loop, one induction variable in the game,
two in ours; 8 bytes shorter). `src/bullet` is therefore that tree
(`notes/PROVENANCE.md`). The `.eh_frame` entry a later file
has for its own, discarded copy of such a function holds the kept copy's address
(0x2053D8 in `btSphereShape.cpp`'s block: 0x1D0178, `btConvexShape::~btConvexShape`
linked from `btCapsuleShape.cpp`): `linkonce_symbols` in `decomp.toml`. A file with a static
object has one `.ctors` and one `.dtors` entry (its `_GLOBAL__I_` / `_GLOBAL__D_`
functions) and a `.bss` piece in program header 1, entered in the `data` segment at
its address in the absolute image (the value in the PRX plus 0x243780); the build
script names the C++ object there and keeps the bss start unaligned
(`tools/configure.py`, `split`). A global variable such a file defines in `.bss` or
`.data` also needs its name in `symbol_addrs.txt` when the remaining assembly refers
to it.

| Range | File | Status |
|---|---|---|
| 0x13C7BC-0x13C970 | `btCollisionObject.cpp` | identical, linked from source |
| 0x141410-0x141484 | `btEmptyCollisionAlgorithm.cpp` | identical, linked from source (with `btCollisionAlgorithm`'s vtable, typeinfo and destructors) |
| 0x141490-0x141DE4 | `btManifoldResult.cpp` | identical, linked from source (`.bss` 0x62C658: `gContactAddedCallback`) |
| 0x141DE4-0x142E74 | `btSphereBoxCollisionAlgorithm.cpp` | identical, linked from source (`.gcc_except_table` 0x20ADF7; `getSphereDistance` since the `div.s` / `sqrt.s` latency, notes/COMPILER.md) |
| 0x142E74-0x1432EC | `btSphereSphereCollisionAlgorithm.cpp` | identical, linked from source (`.gcc_except_table` 0x20AE41) |
| 0x1432EC-0x143834 | `btSphereTriangleCollisionAlgorithm.cpp` | identical, linked from source (`.gcc_except_table` 0x20AE89) |
| 0x143834-0x143B14 | `btBoxShape.cpp` | identical, linked from source (its inline functions 0x1D95DC-0x1DA010, `getEdge` at 0x1D95FC) |
| 0x1449BC-0x145360 | `btCapsuleShape.cpp` | identical, linked from source |
| 0x145360-0x1456CC | `btCollisionShape.cpp` | identical, linked from source |
| 0x146658-0x146C1C | `btConvexInternalShape.cpp` | identical, linked from source |
| 0x146C1C-0x147468 | `btCylinderShape.cpp` | identical, linked from source (`.gcc_except_table` 0x20AFFB; its copies of `btBoxShape::getPreferredPenetrationDirection` / `getPlaneEquation` are the discarded ones) |
| 0x14E870-0x14F17C | `btPolyhedralConvexShape.cpp` | identical, linked from source (`getAabb` since the compiler's pseudo register numbers, notes/COMPILER.md) |
| 0x14F17C-0x14F4E8 | `btSphereShape.cpp` | identical, linked from source |
| 0x14F4E8-0x14FA60 | `btStaticPlaneShape.cpp` | identical, linked from source (`.gcc_except_table` 0x20B118) |
| 0x1501AC-0x15020C | `btTriangleCallback.cpp` | identical, linked from source |
| 0x15020C-0x150AAC | `btTriangleMeshShape.cpp` | identical, linked from source (`.gcc_except_table` 0x20B150; `getAabb` since the compiler's pseudo register numbers) |
| 0x15183C-0x15186C | `btConvexCast.cpp` | identical, linked from source |
| 0x15186C-0x151954 | `btGjkEpaPenetrationDepthSolver.cpp` | identical, linked from source |
| 0x151954-0x15256C | `btGjkPairDetector.cpp` | identical, linked from source (`.bss` 0x62C660: `gNumGjkChecks`, `gNumDeepPenetrationChecks`; `getClosestPoints` since the `div.s` / `sqrt.s` latency) |
| 0x15256C-0x153080 | `btPersistentManifold.cpp` | identical, linked from source (`.data` 0x2479D4: `gContactBreakingThreshold`; `.bss` 0x62C668: `gContactProcessedCallback`, `gContactDestroyedCallback`) |
| 0x153080-0x153B64 | `btRaycastCallback.cpp` | identical, linked from source (`.gcc_except_table` 0x20B1C4) |
| 0x156060-0x1587B0 | `btConeTwistConstraint.cpp` | identical, linked from source (`solveConstraint` since the `div.s` / `sqrt.s` latency; its copy of `btPlaneSpace1` is the discarded one, `btStaticPlaneShape.cpp`'s is kept) |
| 0x15CB88-0x15DC58 | `btPoint2PointConstraint.cpp` | identical, linked from source (`buildJacobian` since the compiler's pseudo register numbers) |
| 0x16392C-0x163CB8 | `btTypedConstraint.cpp` | identical, linked from source (`.ctors` 0x1E4A54, `.dtors` 0x1E4B10, `.gcc_except_table` 0x20B25E, `.bss` 0x64C680: `static btRigidBody s_fixed`) |
| 0x16AC90-0x16ACC0 | `btCollisionAlgorithm.cpp` | identical, linked from source |
| 0x16ACC0-0x16ACF0 | `btDispatcher.cpp` | identical, linked from source |
| 0x16ACF0-0x16BA14 | `SphereTriangleDetector.cpp` | identical, linked from source |
| 0x16BA14-0x16BF34 | `btBoxBoxCollisionAlgorithm.cpp` | identical, linked from source (`.gcc_except_table` 0x20B40D) |
| 0x1717A0-0x172204 | `btConvexConvexAlgorithm.cpp` | identical, linked from source (`.gcc_except_table` 0x20B5CD, `.bss` 0x64C878: `bool disableCcd`) |
| 0x17478C-0x1747EC | `btConcaveShape.cpp` | identical, linked from source |
| 0x1747EC-0x174E40 | `btGjkConvexCast.cpp` | identical, linked from source (`.gcc_except_table` 0x20B718) |
| 0x174E40-0x17662C | `btGjkEpa2.cpp` | identical, linked from source (`.rodata` 0x20E0D0, `.gcc_except_table` 0x20B732) |
| 0x17662C-0x17864C | `btContactConstraint.cpp` | identical, linked from source (`resolveSingleBilateral` since the compiler's pseudo register numbers) |

Gaps not listed (0x19D5E8-0x19DC20, 0x1A2690-0x1A627C, 0x1A9620-0x1AC2A0,
0x1ACE24-0x1AF818) hold more library code without telling strings; 0x1AC080, in the
third, is the zlib-format decompressor that calls the inflate routine.

Alignment padding: the objects of this link have a 4-aligned `.text`, so padding marks
no unit bound; the single exception is before the inflate routine (above). Next: refine
each bound (alignment padding, call graph), identify the unidentified ranges, and
compare `UCUS98746`.
