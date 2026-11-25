# Matching notes

Techniques that made a function match, with the function that proves each one.
Game code is C++; until names are proven, functions and data are declared
`extern "C"` with their address names.

## Game code (C++, `-O2`)

- **Aggregates by value (PSP EABI).** A 12-byte `{float x, y, z}` returned by value
  goes through a hidden pointer in `$a0`; passed by value it goes by invisible
  reference to a caller's stack copy. `f(g(), h(), c)` with both results passed by
  value gives the game's `lw`/`sw` copies (0x51C, 0x5B4). A `const Vec3 &`
  parameter gives the same code where no copy is needed (0x64C, 0xBD4).
- **Temporaries, not named locals**, for intermediate results: GCC 4.0 places
  temporaries before named locals on the stack (0x64C: every slot wrong with named
  locals).
- **Commutative `mul.s` operands swapped, source order without effect**: fold orders
  the operands by SSA version; changing an access form (`v[1]` through an inline
  `operator[]` instead of `v.y`) moves the numbering (0x64C; `-fdump-tree-optimized`
  shows the order).
- **A value reloaded from the stack before each of two calls, where the game keeps it
  in `$f20`**: copy it to a local float before the calls (0x964).
- **`beqzl` with the result in its delay slot at the end of a test chain**: a result
  variable (`r = false; if (call) { r = true; if (cond) r = false; } return r;`), not
  early returns (0x964).
- **`bool` result with both tests branching to a shared `return 0`**: `return !(x || y);`
  or `if (!x && !y) return 1; return 0;` (0xAF8); compute the four float operands into
  locals before the tests, or they are scheduled between the compares.
- **A chain of `beq` instead of a jump table**: an `if` / `else if` chain in the
  game's test order, not a `switch` (0xF24).
- **A call result ignored, but the next `lui` lands in `$v1`**: the callee returns a
  value; declare it `int` (0xDAC).
- **A data address that is an element of a table** (`%lo(D_002437F0)` = table + 0x60):
  declare it as its own symbol (0xDAC). Splat names it so; the link resolves both.
- **Type-based aliasing**: an `enum` type for a tested field, `void *` for fields
  stored as zero, `bool` instead of `unsigned char` for a byte global: each removes
  a reload the game does not have, or moves a load above stores (0x113C, 0x1E60,
  0x14B8).
- **Merging a unit: one type per field.** Where two functions disagree on a field's
  type (`void *` and `int` at the same offset), an `enum` type often satisfies both:
  `Action::type` and `Cursor::state` are enums in `unit_00000D6C` (0x113C and 0x1E60
  identical again after the merge; `int` gives 41 % / 93 %).
- **A loop counter stored once after the loop**: a plain `for` with `member++` in
  the body (0x113C).
- **`%hi` kept in a saved register and `%lo` added at each use**: declare the global
  as an object (`extern Obj D;`, pass `&D`), not as an array (0x1470).
- **A loop bound folded into a countdown that the game does not have**: store the
  pointer difference in an `int` first (`int n = p - start; if (n >= 3) break;`, 0x12B4).
- **Stepping back through a field** (`o->cur--; ... f(o->cur)`), not through a local
  copy, gives the game's register for the pointer (0x1E60).
- **The game's `else` block laid out of line**: write the condition so that block
  is the `else` branch (0x1E60).

## Not source problems

- In a NonMatching (report-only) unit, the static initialisation functions keep the
  compiler's names (`_Z41__static_initialization_and_destruction_0ii`,
  `_GLOBAL__I_<first global>`), which objdiff does not pair with `func_<addr>`;
  an `__asm__` label on a constructor declaration is ignored by 4.0.4 (C1/C2 clones).
- A float literal compared alone shows `[.rodata.cst4]` against `D_<addr>`: the
  literal's place, solved by linking the whole unit (its `.rodata.cst4` block).
- `lui $vX, 0x25` without `%hi`: splat could not pair a `%hi` with its `%lo` across a
  branch (a `lui` in a delay slot, 0xEF0 in 0xDAC); same encoding.
- A string address built by `lui`/`addiu` with no relocation name: a string literal
  with no symbol of its own inside another string block (0x216420 in 0x12B4).
