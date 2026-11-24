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

### Adopted

2026-11-24: `4.0.4c` at `-O2 -G0` is the `[compiler]` of `decomp.toml`. Built in WSL
(Ubuntu 24.04) with the same recipe, its code generation fingerprint equals the kit's
(`check.sh`: `FINGERPRINT_OK`; the binaries differ only by the embedded install path)
and the probe gives 12/12 locally. The CI build job builds it on a cache miss, checks
the fingerprint and runs the probe on every push.
