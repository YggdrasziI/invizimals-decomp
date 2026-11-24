# The EBOOT as a PRX

## Getting the module

The disc's `PSP_GAME/SYSDIR/BOOT.BIN` is 3231028 bytes of zeros. `SYSDIR/EBOOT.BIN` is
the encrypted form (`~PSP` header, tag `0xD9160BF0`, type 2, compression attribute 0,
ELF size 3231028 at header offset 0x28). `tools/extract.py` decrypts it with
pspdecrypt 1.0 ("Decryption successful for tag D9160BF0 with type 2") into
`orig/UCES01241/EBOOT.PRX`. The decrypted size equals the ELF size of the header.

## Layout

`py -3.13 tools/psp/prx_info.py orig/UCES01241/EBOOT.PRX`:

- ELF type 0xFFA0 (relocatable PRX), flags 0x10A23001, entry 0x114 (`module_start`).
- Program header 0: file 0xC0, vram 0x0, 0x243750 bytes (code and read-only data).
- Program header 1: file 0x243840, vram 0x243780, filesz 0x4A18, memsz 0x40FEF0
  (`.data`, `.data.0001`, then `.sbss` (empty), `.bss`, `.bss.0001`, `.bss.0002`).
- Program header 2: file 0x248260, the relocation tables (type 0x700000A0), memsz 0.
- In both loaded segments, file offset = vram + 0xC0; the 0x30 bytes between the two
  (file 0x243810-0x243840) are alignment of program header 1 (align 64).
- After the tables: `.comment` (0x3116B8), `.shstrtab`, section headers (0x3125AC),
  empty `.gcc_compiled_long32` at the end of the file (0x314D34).
- `.rodata` is followed by 104 sections `.rodata.0001`-`.rodata.0104` (and `.data.0001`,
  `.bss.0001`, `.bss.0002`), each 8-aligned, some with their own relocation table.

## Module info and gp

`.rodata.sceModuleInfo` (0x1E40B8): attributes 0, version 1.1, name `Invizimals`,
`gp` 0xCA10. The gp word has an R_MIPS_32 relocation into program header 1, so the
running value is 0x243780 + 0xCA10 = 0x250190, which is `.sbss` (0x2481A0) + 0x7FF0.
`gp_value` of the splat config is that absolute value.

## Relocations

`tools/psp/prx_reloc.py` puts the program-header-1 references in absolute form before
splat and back after the link. A relocation's offset is relative to the segment it
names (`offset_seg`): the `.data` tables use program header 1, so their file
position is 0x243840 + offset. (The tool used vram-relative positions until
2026-11-24, which is the same thing for a segment at vram 0 only; with this EBOOT the
`.data` relocations landed in `.text` and the result depended on the hash seed.)

`tools/psp/reloc_overrides.py` writes `config/UCES01241/reloc_addrs.eboot.txt`
(4186 entries): symbol guesses of spimdisasm without a relocation become plain
values. With vram 0, small constants and struct offsets look like code addresses.

One case remains (`config/UCES01241/symbol_addrs.txt`): `swc1 $f0, -0x3874($s1)` at
0xF4460 is not relocated (the override is right), yet spimdisasm still creates a data
symbol at 0xC78C, a branch target inside `func_0000C6D0`. The branch then refers to
`.L0000C78C` while the label is emitted as `D_0000C78C`, and the link fails. Naming
the address (`lbl_0000C78C`, `type:label`) gives both the same name. `UCUS98746` has
the same case 8 bytes lower: `swc1 $f0, -0x387C($s1)` at 0xF9DE8 and the `bnez` at
0xC764 both point at 0xC784 (`lbl_0000C784` in `config/UCUS98746/symbol_addrs.txt`).
Adding a version: write its `splat.eboot.yaml` from `tools/psp/prx_info.py`, run
`prx_symbols.py`, split, `reloc_overrides.py`, split again, then name such labels.

## Imports and exports

- `.lib.ent`: the module's own export library with the standard SDK NIDs:
  `module_start` 0x114, `module_stop` 0x4EC, `module_info`, `module_sdk_version`
  (0x20BC60), the two thread parameters.
- `.lib.stub`: 42 libraries, 340 functions (`sceAudio`, `sceGe_user`, `sceUsbCam`,
  `sceMpeg`, `sceNetAdhoc*`, `sceNp*`, `sceUtility`...). 4 NIDs have no name in
  `tools/psp/nids.txt`.
- `tools/psp/prx_symbols.py` writes them to `config/UCES01241/symbol_addrs.eboot.txt`.

## Build pipeline

```
orig EBOOT.PRX --prx_reloc --absolute--> build/UCES01241/EBOOT.abs.prx
               --splat--> asm/UCES01241/eboot/*.s, build/UCES01241/bin/elf_{header,tail}.bin,
                          build/UCES01241/eboot.ld
*.s --tools/psp/assemble.py (vfpu_words + as)--> .o ; *.bin --objcopy -I binary--> .o
ld -T eboot.ld --> eboot.elf --objcopy -O binary--> eboot.bin (= the absolute image)
   --prx_reloc --relative--> EBOOT.PRX --sha1--> OK
```

splat groups the entries of one segment by section type in `section_order`, so the
yaml keeps each type contiguous: program header 0 is text (`.init` to `.fini`, then the
system stubs) followed by rodata (everything from `.lib.ent.top` to the end of the
segment, `.ctors` and `.eh_frame` included); program header 1 is data then bss.
