# Splits

Bounds of the splat pieces of `config/UCES01241/splat.eboot.yaml` and what proves
them. A split that builds proves the bytes, not a translation unit.

## Current pieces (2026-11-24)

| File offset | vram | Piece | Contents |
|---|---|---|---|
| 0x0 | | `bin elf_header` | ELF header, program headers |
| 0xC0 | 0x0 | `asm text` | `.init`, `.text` from 0x30 |
| 0x18D90 | 0x18CD0 | `c unit_00018CD0` | `func_00018CD0` only; bounds are the function's own (hypothesis: the translation unit is not known) |
| 0x18DB0 | 0x18CF0 | `asm text_00018CF0` | rest of `.text` (to 0x1E3294), `.fini` |
| 0x1E3370 | 0x1E32B0 | `asm scestub` | `.sceStub.text` |
| 0x1E3E10 | 0x1E3D50 | `rodata lib` | `.lib.ent*`, `.lib.stub*`, `.rodata.sceModuleInfo` |
| 0x1E41AC | 0x1E40EC | `rodata sceresident` | `.rodata.sceResident` |
| 0x1E44B4 | 0x1E43F4 | `rodata scenid` | `.rodata.sceNid` |
| 0x1E4A04 | 0x1E4944 | `rodata ctors` | `.ctors`, `.dtors`, `.jcr` |
| 0x1E4BDC | 0x1E4B1C | `rodata eh_frame` | `.eh_frame`, `.gcc_except_table` |
| 0x20B840 | 0x20B780 | `rodata rodata` | `.rodata`, `.rodata.0001`-`.0106`, padding |
| 0x243840 | 0x243780 | `data data` | `.data`, `.data.0001` |
| (bss) | 0x248198 | `bss bss` | `.sbss`, `.bss`, `.bss.0001`, `.bss.0002` (0x40B4D8) |
| 0x248258 | | `bin elf_tail` | relocation tables, `.comment`, `.shstrtab`, section headers |

## Region map

Not made yet: the next step is the inventory of `.text` (libraries recognised by their
strings or by the SDK, game code), with the evidence for each bound.
