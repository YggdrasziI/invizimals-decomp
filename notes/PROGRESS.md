# Progress log

| Date | Commit | Matched code | Matched functions | Units (matching / total) | Milestone |
|---|---|---|---|---|---|
| 2026-11-24 | | 0 bytes | 0 | 0 / 0 | All-assembly build of the decrypted EBOOT matches (`build/UCES01241/EBOOT.PRX: OK`) |
| 2026-11-24 | | 32 bytes | 1 | 1 / 1 | First function linked from C: `func_00018CD0` (`src/unit_00018CD0.c`), GCC 4.0.4c `-O2 -G0` |
| 2026-11-25 | | 17408 bytes (0.88 %) | 204 | 18 / 47 | 16 Bullet files linked from the original sources (trunk 2.71 with the game's `LinearMath` changes), compiler `4.0.4a` with the retail scheduling latencies |
| 2026-11-25 | | 18100 bytes (0.91 %) | 215 | 19 / 50 | `btTypedConstraint.cpp` linked: first file with a static object (`.ctors`, `.dtors` and `.bss` pieces) |
| 2026-11-25 | | 23632 bytes (1.19 %) | 288 | 22 / 54 | 20 Bullet files linked: `btBoxShape.cpp`, `btConvexConvexAlgorithm.cpp` and `btSphereTriangleCollisionAlgorithm.cpp` identical with the retail branch cost (compiler `4.0.4a`) |
