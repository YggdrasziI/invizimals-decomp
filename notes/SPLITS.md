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
| 0x13A2C0-0x17661C | Bullet Physics | source paths `BulletCollision/...cpp`, `BulletDynamics/...cpp`, `./LinearMath/btAlignedAllocator.h` referenced in address order from 0x13A2C0 to 0x174200; end bound: start of the next run without FDEs |
| 0x17661C-0x18DE70 | expat 2.0.0 (C) | no FDEs; `expat_2.0.0` (0x179384), `xmlparse.c` assertion (0x178FE8), XML namespace URIs and DTD keywords (0x1797B8-0x18DA8C); start: every function up to 0x1764D0 is called from Bullet, from 0x17661C on the calls stay inside; end: the only alignment padding of `.text` (3 nops at 0x18DE64) |
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
| 0x17661C-0x17864C | unidentified | no upstream function found here |
| 0x17864C-0x1805B8 | `xmlparse.c` | 114/116 functions identical; `XML_ParserFree` (register allocation in the tag list loop) and `doContent` (93.8 %) differ |
| 0x1805B8-0x181F90 | `xmlrole.c` | identical as a whole file (`.text` 0x19D8 bytes, `.rodata` 0x324 bytes at 0x20E624); linked from source |
| 0x181F90-0x18DE70 | `xmltok.c` | 97/102 functions identical; `normal_scanPi`, `little2_sameName` and the three `nameMatchesAscii` differ (block order, registers) |

Gaps not listed (0x19D5E8-0x19DC20, 0x1A2690-0x1A627C, 0x1A9620-0x1AC2A0,
0x1ACE24-0x1AF818) hold more library code without telling strings; 0x1AC080, in the
third, is the zlib-format decompressor that calls the inflate routine.

Alignment padding: the objects of this link have a 4-aligned `.text`, so padding marks
no unit bound; the single exception is before the inflate routine (above). Next: refine
each bound (alignment padding, call graph), identify the unidentified ranges, and
compare `UCUS98746`.
