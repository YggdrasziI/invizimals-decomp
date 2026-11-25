#!/bin/sh
# Build psp-gcc-4.0.4a: the LBP-PSP-Decomp kit's psp-gcc-4.0.4c plus
# tools/gcc/block-move-4.0.4.diff (notes/COMPILER.md, "Block copies"),
# tools/gcc/allegrex-sched-4.0.4.diff ("Scheduling latencies") and
# tools/gcc/branch-cost-4.0.4.diff ("Branch cost").
#
# Run after the kit's `build.sh 4.0.4c`, with the same environment: it copies the
# 4.0.4c work tree ($LBP_GCC4_WORK/psp-gcc-4.0.4c), applies the diffs, rebuilds cc1 and
# cc1plus there, and installs $LBP_GCC4_ROOT/psp-gcc-4.0.4a as a copy of psp-gcc-4.0.4c
# with these two programs.
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=${LBP_GCC4_ROOT:-$HOME/lbp-gcc4}
WORK=${LBP_GCC4_WORK:-$ROOT/work}
JOBS=${JOBS:-2}
# host flags of the kit's build.sh (old GCC on a modern host compiler)
HF="-O1 -g0 -std=gnu89 -fcommon -fpermissive -w -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0"

[ -d "$WORK/psp-gcc-4.0.4c/build/gcc" ] || { echo "no $WORK/psp-gcc-4.0.4c: run the kit's build.sh 4.0.4c first" >&2; exit 1; }
[ -x "$ROOT/psp-gcc-4.0.4c/bin/psp-gcc" ] || { echo "no $ROOT/psp-gcc-4.0.4c" >&2; exit 1; }

W=$WORK/psp-gcc-4.0.4a
P=$ROOT/psp-gcc-4.0.4a
rm -rf "$W" "$P"
cp -a "$WORK/psp-gcc-4.0.4c" "$W"
(cd "$W/gcc-4.0.4" && patch -p1 -F0 --no-backup-if-mismatch < "$HERE/block-move-4.0.4.diff" > ../patch-block-move.log)
(cd "$W/gcc-4.0.4" && patch -p1 -F0 --no-backup-if-mismatch < "$HERE/allegrex-sched-4.0.4.diff" > ../patch-allegrex-sched.log)
(cd "$W/gcc-4.0.4" && patch -p1 -F0 --no-backup-if-mismatch < "$HERE/branch-cost-4.0.4.diff" > ../patch-branch-cost.log)
(cd "$W/build/gcc" && make -j"$JOBS" cc1 cc1plus MAKEINFO=true CFLAGS="$HF" > ../make-4.0.4a.log 2>&1) \
  || { tail -20 "$W/build/make-4.0.4a.log"; exit 1; }
cp -a "$ROOT/psp-gcc-4.0.4c" "$P"
cp "$W/build/gcc/cc1" "$W/build/gcc/cc1plus" "$P/libexec/gcc/psp/4.0.4/"
echo "BUILD_OK psp-gcc-4.0.4a $("$P/bin/psp-gcc" --version | head -1)"
