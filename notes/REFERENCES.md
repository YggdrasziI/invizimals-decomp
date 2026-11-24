# References

## Tools

- [ethteck/splat](https://github.com/ethteck/splat) (`splat64[mips]` 0.50.1, MIT).
- [Decompollaborate/spimdisasm](https://github.com/Decompollaborate/spimdisasm) and
  rabbitizer: disassembler used by splat.
- [decompals/binutils-mips-ps2-decompals](https://github.com/decompals/binutils-mips-ps2-decompals)
  v0.10: `as`/`ld`/`objcopy`, without Allegrex or VFPU (`tools/psp/vfpu_words.py`).
- [encounter/objdiff](https://github.com/encounter/objdiff): diff and report.
- [matt-kempster/m2c](https://github.com/matt-kempster/m2c): MIPS to C draft (not
  the PyPI `m2c`, an unrelated project).
- [John-K/pspdecrypt](https://github.com/John-K/pspdecrypt) 1.0 (GPL-3.0): EBOOT
  decryption, run by `tools/extract.py`.
- [decompme/compilers](https://github.com/decompme/compilers) and
  [decomp.me](https://decomp.me): PSP compilers for the probes.
- [pspdev/pspdev](https://github.com/pspdev/pspdev): homebrew toolchain; its GCC
  4.0.2 build (`PSPDEV 20051022`) is named in the module's `.comment`.
- [hrydgard/ppsspp](https://github.com/hrydgard/ppsspp) (GPL, read only): PRX loader
  behaviour, HLE NID tables, debugger.
- [kotcrab/ghidra-allegrex](https://github.com/kotcrab/ghidra-allegrex): Ghidra PRX
  loader and Allegrex/VFPU support, outside the repository.

## Reference projects

- The owner's PSP projects: mhfu-decomp (Metrowerks, static `BOOT.BIN`) and
  jd-tlf-decomp (SNC, relocatable PRX, the closest layout).
