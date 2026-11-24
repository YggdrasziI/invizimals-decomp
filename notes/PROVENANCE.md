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
