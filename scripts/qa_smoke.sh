#!/usr/bin/env bash
# scripts/qa_smoke.sh — Human-AI QA-loop smoke test for commits bcb2b79 + 66e594e
#
# Use this as the canonical next-turn host command. Build, ctest, smoke, and
# produce evidence files the AI can read back without SSH.
#
# Usage (on piotro@prizm-laptop):
#   cd ~/.openclaw/workspace/npp-qt
#   bash scripts/qa_smoke.sh
#
# Output goes to:
#   /tmp/build.log         — cmake build stdout+stderr
#   /tmp/ctest.log         — ctest summary
#   /tmp/smoke.log         — 8s binary run + Qt platform plugin logs
#   /tmp/recent_files.txt  — captured recent-files list after smoke

set -uo pipefail

ROOT="/home/piotro/.openclaw/workspace/npp-qt"
BUILD="$ROOT/build"
BIN="$BUILD/NotepadMinusMinusQt"

cd "$ROOT" || exit 1

echo "=== [1/4] cmake build ==="
cmake --build "$BUILD" -j"$(nproc)" >/tmp/build.log 2>&1
BUILD_RC=$?
tail -20 /tmp/build.log
echo "build exit: $BUILD_RC"

echo ""
echo "=== [2/4] ctest (with offscreen Qt platform) ==="
QT_QPA_PLATFORM=offscreen ctest --test-dir "$BUILD" --output-on-failure >/tmp/ctest.log 2>&1
CTEST_RC=$?
tail -25 /tmp/ctest.log
echo "ctest exit: $CTEST_RC"

echo ""
echo "=== [3/4] 8s binary smoke ==="
QT_QPA_PLATFORM=offscreen timeout 8s "$BIN" >/tmp/smoke.log 2>&1
SMOKE_RC=$?
echo "smoke exit: $SMOKE_RC  (124 = timeout = good, app stayed up)"
tail -20 /tmp/smoke.log

echo ""
echo "=== [4/4] evidence bundle ==="
echo "All logs: /tmp/build.log, /tmp/ctest.log, /tmp/smoke.log"
echo "For the next AI turn, paste these three files into the conversation,"
echo "or run: cat /tmp/build.log /tmp/ctest.log /tmp/smoke.log"

# Final verdict
if [[ "$BUILD_RC" -eq 0 && "$CTEST_RC" -eq 0 && "$SMOKE_RC" -eq 124 ]]; then
  echo ""
  echo "✅ ALL GREEN — push with: git -C $ROOT push origin master"
  exit 0
else
  echo ""
  echo "❌ INVESTIGATE — paste the failing log"
  exit 1
fi
