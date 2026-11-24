Invizimals (PSP)
================

> [!IMPORTANT]
> AI Disclosure:
> AI was used for the following tasks:
> - Writing commentary (primarily translating into English and formatting).
> - Documenting and citing the various sources used in the project (see PROVENANCE.md, REFERENCE.md, and more generally the files contained within the "notes" directory).
> - Writing code (C, C++, Python, etc.—I used it as a tool) and reviewing my own work (yes, some code was written by AI, but it was verified by a human; I didn't simply type "Claude, decompile this for me").

A work-in-progress matching decompilation of Invizimals for the PlayStation
Portable, built with [splat](https://github.com/ethteck/splat).

> [!NOTE]
> No port to PC or other platforms is planned for the time being, but should that change, the link will appear here

This repository does **not** contain any game assets or assembly whatsoever. An
existing copy of the game is required.

Supported versions (the ISO is the only input; `PSP_GAME/SYSDIR/BOOT.BIN` is zeroed on
these discs, so `tools/extract.py` takes `PSP_GAME/SYSDIR/EBOOT.BIN` out of it and
decrypts it, and the build rebuilds that decrypted file):

Version   | Release | ISO SHA1 | Built decrypted `EBOOT.BIN` SHA1
--------- | ------- | -------- | --------------------------------
UCES01241 | Invizimals (Europe), `DISC_VERSION 1.00` | `5a0a24e2d45db3da573a97ae1a86ce3ea59f6fa7` | `4868a1538f992a1a6f2c575ddc51f81778932bd2`
UCUS98746 | Invizimals (USA), `DISC_VERSION 1.02` | `836f5df73c145b58bc02bfb24a8340d86f7bcb93` | `5beefc7bdad3c43dbe3bf271de00e1a43b048118`

The work targets `UCES01241`; `UCUS98746` is paused: its all-assembly build matches,
nothing more is done on it for now.

Progress
========

| Release | Disc ID | Progress |
|---|---|---|
| Invizimals (Europe) | `UCES01241` | not measured yet (no C yet) |
| Invizimals (USA) | `UCUS98746` | not measured yet (paused) |

Dependencies
============

- [Python](https://www.python.org/downloads/) 3.10+ in `%PATH%`, then
  `pip install -r tools/requirements.txt`.
- [ninja](https://github.com/ninja-build/ninja/releases) in `%PATH%`
  (`pip install ninja` works too).
- [7-Zip](https://www.7-zip.org/) to extract the ISO.
- The C compiler: GCC 4.0.4 for the PSP built from GNU sources with the
  [LBP-PSP-Decomp](https://github.com/Jstar269/LBP-PSP-Decomp) kit
  (`toolchain/gcc4`, build `4.0.4c`, as `.github/workflows/build.yml` does), installed
  in `~/lbp-gcc4` of a Linux system. It is a Linux program: on Windows, the build runs
  it in WSL (Ubuntu 24.04); the rest of the tooling is native. Only needed once there
  is C in `src/`.

[pspdecrypt](https://github.com/John-K/pspdecrypt) is downloaded by
`tools/extract.py`; the MIPS binutils and objdiff-cli by the first `ninja` run.

Building
========

1. Copy the ISO to `orig/<version>/` (any file name ending in `.iso`).
2. Run:

   ```
   git submodule update --init
   python tools/extract.py --version <version>
   python tools/configure.py --version <version>
   ninja
   ```

   A build ends with `build/<version>/EBOOT.PRX: OK`. The version defaults to
   `UCES01241`.
3. `ninja report` writes the objdiff report (`build/<version>/report.json`);
   `objdiff.json` is there for the objdiff GUI.

Documentation
=============

- `SPEC.md`: target module, toolchain, build pipeline, acceptance criteria, naming.
- `notes/`: PRX layout, compiler evidence, region map, matching techniques,
  references and provenance of reused code, progress log.
