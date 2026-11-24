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

## GCC 4.0.x build kit

The probe workflow `.github/workflows/probe-gcc4.yml` fetches `toolchain/gcc4` of
[Jstar269/LBP-PSP-Decomp](https://github.com/Jstar269/LBP-PSP-Decomp) at commit
`754764d14759c58969bfe29cdc9ab5346255b7ee` (kit files GPL-3.0-or-later, GCC and the
pspdev patches GPL-2.0-or-later) and runs its `build.sh` in CI. Nothing of the kit is
copied into this repository. It also downloads the pspdev release `v20261001` (for
`psp-ld`) and the decomp.me `psp-gcc-1.3.1` archive (for the Sony assembler).
