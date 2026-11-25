# Invizimals (PSP) decompilation — specification

## Goal

Rebuild Invizimals (PlayStation Portable) from C/C++ source written from scratch so
that, compiled with the original compiler and flags, it produces a game executable
identical byte for byte to the original (decrypted) file.

Out of scope: a port, game assets, any redistribution of game files.

## Versions

| Version | Release | Status |
|---|---|---|
| `UCES01241` | Europe (13 languages), `DISC_VERSION 1.00`, `PSP_SYSTEM_VER 6.00` | active |
| `UCUS98746` | USA, `DISC_VERSION 1.02`, `PSP_SYSTEM_VER 6.20`, `APP_VER 01.00` | paused: all-assembly build matches |

Each version has its own `config/<version>/` and `orig/<version>/`; decompilation
work targets the European one.

| Version | ISO SHA-1 | Decrypted `EBOOT.BIN` | Notes |
|---|---|---|---|
| `UCUS98746` | `836f5df73c145b58bc02bfb24a8340d86f7bcb93` (1615003648 bytes) | 3281928 bytes, SHA-1 `5beefc7bdad3c43dbe3bf271de00e1a43b048118` | `BOOT.BIN` zeroed; `EBOOT.BIN` 3282272 bytes, tag `0xD91613F0`. Entry 0x108, `.text` 0x24-0x1EA798 (0x7510 bytes longer), program header 1 at vram 0x24CA00, `$gp` 0x259450, 43 import libraries (345 functions: `sceGameUpdate` added), one `.bss.NNNN` fewer. `.comment`: the same GCC 4.0.4 (psp-2.9.0, psp-2.7.3) and PSPDEV 4.0.2 strings, but `GCC: (GNU) 3.3.6+allegrex-2.2.2-psp-1.20.1.internal` instead of `psp-1.15.3.internal` (an object built with a newer SDK, probably a library: hypothesis) |

The two USA dumps named "(En,Fr,Es)" and "(En,Fr,Es,Pt)" are the same file (same
ISO SHA-1).

## Target (UCES01241)

| Item | Value | Evidence |
|---|---|---|
| Disc ID | `UCES01241` (Europe), `DISC_VERSION 1.00`, `PSP_SYSTEM_VER 6.00`, publisher SCEE, title `inviZimals` | `PSP_GAME/PARAM.SFO`, `UMD_DATA.BIN` |
| ISO SHA-1 | `5a0a24e2d45db3da573a97ae1a86ce3ea59f6fa7` (1613922304 bytes), "(Europe) (En,Fr,De,Es,It,Nl,Pt,Sv,No,Da,Fi,Pl,Ru)"; a CSO copy of the same release decompresses to the same bytes | `tools/extract.py` |
| `SYSDIR/BOOT.BIN` | 3231028 bytes, all zero | file contents |
| `SYSDIR/EBOOT.BIN` | 3231376 bytes, encrypted `~PSP`, tag `0xD9160BF0` (type 2), not compressed, ELF size 3231028 | `~PSP` header, pspdecrypt |
| Target module | `EBOOT.BIN` decrypted: `orig/UCES01241/EBOOT.PRX`, 3231028 bytes, SHA-1 `4868a1538f992a1a6f2c575ddc51f81778932bd2` | `config/UCES01241/build.sha1` |
| Module | name `Invizimals`, version 1.1, attributes 0 | `.rodata.sceModuleInfo` |
| Format | relocatable PRX (ELF type 0xFFA0, flags 0x10A23001, entry 0x114), 3 program headers (the third holds the relocation tables), 253 section headers | `tools/psp/prx_info.py` |
| Program header 0 | file 0xC0, vram 0x0, 0x243750 bytes: `.init`, `.text` 0x30-0x1E3294, `.fini`, `.sceStub.text`, `.lib.*`, `.rodata.sce*`, `.ctors`, `.dtors`, `.jcr`, `.eh_frame`, `.gcc_except_table`, `.rodata` and `.rodata.0001`-`.0106` | section headers |
| Program header 1 | file 0x243840, vram 0x243780, 0x4A18 bytes in the file, 0x40FEF0 in memory: `.data`, `.data.0001`, `.sbss` (empty), `.bss`, `.bss.0001`, `.bss.0002`; `$gp` = module info 0xCA10 + segment base = 0x250190 (`.sbss` + 0x7FF0) | section headers, module info |
| Entry | `module_start` 0x114, `module_stop` 0x4EC | `.lib.ent` (standard SDK export NIDs) |
| Imports | 42 system libraries, 340 functions (4 NIDs without a known name) | `.lib.stub`, `config/UCES01241/symbol_addrs.eboot.txt` |
| Relocations | `.rel.text`: 39523 R_MIPS_26, 20906 R_MIPS_HI16, 24563 R_MIPS_LO16; data tables: R_MIPS_32 (`.rodata`, `.eh_frame`, `.ctors`, `.data`...). References into program header 1 are stored relative to its base | `tools/psp/prx_info.py`, `notes/PRX.md` |
| `.comment` | `GCC: (GNU) 4.0.4+allegrex-2.2.3-psp-2.9.0`, `GCC: (GNU) 3.3.6+allegrex-2.2.2-psp-1.15.3.internal`, `GCC: (GNU) 4.0.4+allegrex-2.2.3-psp-2.7.3`, `GCC: (GNU) 4.0.2 (PSPDEV 20051022)`; section `.gcc_compiled_long32` | `.comment` at file 0x3116B8 |

Other files of the disc: `USRDIR/data.mpak` (517 MB of data), 66 `.pmf` videos,
`SYSDIR/UPDATE` (system update). No other code module was found on the disc.

## Toolchain

Evidence and the candidate analysis go in `notes/COMPILER.md`.

- pspdecrypt **1.0** ([John-K/pspdecrypt](https://github.com/John-K/pspdecrypt),
  GPL-3.0), downloaded and run by `tools/extract.py` (`tools/psp/download_tool.py`).
- splat **0.50.1** (`splat64[mips]`, spimdisasm 1.42.4): assembly, data and linker
  script from `config/UCES01241/splat.eboot.yaml`.
- binutils from [decompals/binutils-mips-ps2-decompals](https://github.com/decompals/binutils-mips-ps2-decompals)
  **v0.10** (`as -march=mips32r2 -mabi=32`, `ld`, `objcopy`), downloaded by `ninja`
  into `tools/binutils/`. VFPU and Allegrex-only instructions are turned into
  `.word` by `tools/psp/vfpu_words.py`.
- objdiff-cli **v3.8.3** (report, diff).
- m2c (pinned commit in `tools/requirements.txt`) for a first C draft.
- Compiler: the `.comment` strings name Sony's GCC 4.0.4 allegrex (psp-2.9.0 and
  psp-2.7.3), GCC 3.3.6 and PSPDEV GCC 4.0.2. Sony's builds are not available; an
  open-source GCC 4.0.4 rebuilt with the pspdev Allegrex patch and the LBP-PSP-Decomp
  tuning (`4.0.4c`) at `-O2 -G0 -mpreferred-stack-boundary=4` reproduces all twelve probe functions
  (`notes/COMPILER.md`). The `[compiler]` of `decomp.toml` is `4.0.4a`: `4.0.4c` plus
  the retail block copies, scheduling latencies, branch cost and host `qsort` (`tools/gcc/*.diff`, built by
  `tools/gcc/build_404a.sh`), a Linux program in `~/lbp-gcc4/psp-gcc-4.0.4a`, built
  from source (CI: cached), run through
  WSL on Windows (`tools/common/linux_run.py`). expat 2.0.0 (`src/expat`) was built
  with the stock PSPDEV GCC 4.0.2 (`4.0.2h`, also built by the kit): its sources set
  their own compiler and flags (`[compiler.dirs]`).

## Build pipeline

1. `tools/extract.py`: ISO SHA-1, `SYSDIR/EBOOT.BIN` out of the ISO (7-Zip),
   decrypted by pspdecrypt into `orig/UCES01241/EBOOT.PRX`, SHA-1 checked.
2. `tools/psp/prx_reloc.py --absolute`: the PRX with every reference to program header 1
   made absolute, so the disassembly gets the right symbols.
3. splat splits that image, with `config/UCES01241/reloc_addrs.eboot.txt` turning back
   into plain values the address guesses the PRX does not relocate
   (`tools/psp/reloc_overrides.py`): ELF header and the tail (relocation tables,
   `.comment`, `.shstrtab`, section headers) as `bin`, program header 0 as text and
   rodata, program header 1 as data + bss.
4. `as` (`tools/psp/assemble.py`), `objcopy -I binary`, `ld` with the splat linker
   script, `objcopy -O binary`: the absolute image.
5. `tools/psp/prx_reloc.py --relative` gives the PRX back; `tools/common/sha1.py` checks
   it.

The relocation tables are copied from the original (`elf_tail.bin`): this holds while
the layout does not move, which a matching build guarantees. Re-encrypting the EBOOT
is out of scope.

## Acceptance criteria

- Every build ends with `build/UCES01241/EBOOT.PRX: OK` (`ninja`), locally and in CI.
- A unit is matching only when objdiff reports 100 % for all its functions and data
  and the build still matches.
- A split that builds proves the bytes, not the translation unit: a TU boundary is
  proven only once the unit is decompiled and matches (`notes/SPLITS.md`).
- Every name, compiler fact or structure layout rests on evidence (strings with their
  address, import/export tables, library signatures, an objdiff diff, a build
  result). Without evidence it is labelled a hypothesis.
- Reused code or data from another project is logged with its commit and license in
  `notes/PROVENANCE.md`.

## Naming conventions

- splat default names stay until proven: `func_<vram>`, `D_<vram>`, `jtbl_<vram>`.
- System imports: the NID's name from `tools/psp/nids.txt` (PPSSPP HLE tables).
- Module exports: the standard SDK names of their NIDs (`module_start`...).
- Game units: `src/unit_<start vram>.c` (or `.cpp`) until a source path is proven.
- Code, comments, notes and commit messages are in English.

## Repository layout

```
README.md SPEC.md LICENSE
decomp.toml               settings read by the tools: versions, module, regions
config/UCES01241/         build.sha1, splat.eboot.yaml, symbol_addrs.txt,
                          symbol_addrs.eboot.txt, reloc_addrs.eboot.txt (generated)
orig/UCES01241/           the ISO, EBOOT.BIN and the decrypted EBOOT.PRX (git-ignored)
asm/                      splat output (git-ignored)
build/                    build output (git-ignored)
src/  include/            sources and headers
tools/                    configure.py, extract.py, requirements.txt
tools/psp/  tools/common/ shared tools (git submodules)
notes/                    reference documentation
.github/workflows/        CI: rebuild and check the PRX in a private image
```
