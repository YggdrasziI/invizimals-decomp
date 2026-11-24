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

It builds the following modules:

Version   | Module | SHA1 hash of the input
--------- | ------ | ------------------------------------------
UCES01241 | ISO (Europe, 13 languages, `DISC_VERSION 1.00`) | `5a0a24e2d45db3da573a97ae1a86ce3ea59f6fa7`
UCES01241 | `SYSDIR/EBOOT.BIN`, decrypted | `4868a1538f992a1a6f2c575ddc51f81778932bd2`

The disc's `SYSDIR/BOOT.BIN` is zeroed, so the target is the encrypted
`SYSDIR/EBOOT.BIN` after decryption (`tools/extract.py`).

Dependencies
============

- [Python](https://www.python.org/downloads/) 3.10+ in `%PATH%`, then
  `pip install -r tools/requirements.txt`.
- [ninja](https://github.com/ninja-build/ninja/releases) in `%PATH%`
  (`pip install ninja` works too).
- [7-Zip](https://www.7-zip.org/) to extract the ISO.
- On Windows use native tooling; WSL or msys2 are not required.

[pspdecrypt](https://github.com/John-K/pspdecrypt) is downloaded by
`tools/extract.py`; the MIPS binutils and objdiff-cli by the first `ninja` run.

Building
========

1. Copy the ISO to `orig/UCES01241/` (any file name ending in `.iso`).
2. Run:

   ```
   git submodule update --init
   python tools/extract.py
   python tools/configure.py
   ninja
   ```

   A build ends with `build/UCES01241/EBOOT.PRX: OK`.
3. `ninja report` writes the objdiff report (`build/UCES01241/report.json`);
   `objdiff.json` is there for the objdiff GUI.

Documentation
=============

- `SPEC.md`: target module, toolchain, build pipeline, acceptance criteria, naming.
- `notes/`: PRX layout, compiler evidence, region map, matching techniques,
  references and provenance of reused code, progress log.
