# Compiler

Nothing is proven yet. This file collects the evidence; the probes decide.

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
