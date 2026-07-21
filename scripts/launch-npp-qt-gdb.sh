#!/usr/bin/env bash
# scripts/launch-npp-qt-gdb.sh — launch NotepadMinusMinusQt under gdb on the
# real desktop (:1) for the Human-AI QA loop.
#
# Usage (on piotro@prizm-laptop):
#   cd ~/.openclaw/workspace/npp-qt
#   bash scripts/launch-npp-qt-gdb.sh
#
# Side effects:
#   - Starts gdbserver on :23456 (TCP)
#   - Launches the app on DISPLAY=:1 (your real desktop session)
#   - Captures stdout/stderr to /tmp/npp-gdb.log
#   - Captures any "APPINST site=instance() n=N" diagnostic writes
#   - Doesn't attach the gdb client itself — that's a separate step
#     (so you can step from your own terminal without racing the launcher)
#
# To attach from a second terminal:
#   gdb ~/.openclaw/workspace/npp-qt/build/NotepadMinusMinusQt
#   (gdb) target remote :23456
#
# To detach and let the app keep running:
#   (gdb) detach
#
# To stop the app + gdbserver:
#   bash scripts/stop-npp-qt-gdb.sh
#   — or —
#   kill $(cat /tmp/npp-gdb.pid)

set -uo pipefail

ROOT="/home/piotro/.openclaw/workspace/npp-qt"
BUILD="$ROOT/build"
BIN="$BUILD/NotepadMinusMinusQt"
LOG=/tmp/npp-gdb.log
GDB_PORT=23456

# Display selection: prefer :1 (where the real session lives), fall back to
# whatever $DISPLAY is set to, fall back to offscreen.
if [[ -z "${DISPLAY:-}" ]] && [[ -S /tmp/.X11-unix/X1 ]]; then
    export DISPLAY=:1
fi

if [[ ! -x "$BIN" ]]; then
    echo "FAIL: $BIN is missing or not executable"
    echo "Run scripts/qa_smoke.sh first to build it"
    exit 1
fi

# Rotate any previous log
mv -f "$LOG" "${LOG}.prev" 2>/dev/null || true

echo "=== launching $BIN under gdbserver on port $GDB_PORT (DISPLAY=$DISPLAY) ==="
echo "    log: $LOG"
echo "    attach: gdb $BIN ; (gdb) target remote :$GDB_PORT"

# gdbserver args:
#   --once: exit after the first client disconnects
#   :PORT: TCP port
#   $BIN: program to run
#   All following args are passed to $BIN
cd "$ROOT" || exit 1
setsid gdbserver --once ":$GDB_PORT" "$BIN" "$@" \
    >"$LOG" 2>&1 &
GDB_PID=$!
echo $GDB_PID > /tmp/npp-gdb.pid

echo "    pid:  $GDB_PID"
echo "    pid file: /tmp/npp-gdb.pid"
echo
echo "Tail the log with:    tail -f $LOG"
echo "Watch APPINST lines:  tail -f $LOG | grep APPINST"
echo "Stop with:            kill \$(cat /tmp/npp-gdb.pid)"