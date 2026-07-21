#!/usr/bin/env bash
# scripts/stop-npp-qt-gdb.sh — stop the gdbserver-launched NotepadMinusMinusQt.
#
# Usage (on piotro@prizm-laptop):
#   bash scripts/stop-npp-qt-gdb.sh

PID_FILE=/tmp/npp-gdb.pid

if [[ ! -f "$PID_FILE" ]]; then
    echo "no pid file at $PID_FILE — nothing to stop"
    exit 0
fi

PID=$(cat "$PID_FILE")
if kill -0 "$PID" 2>/dev/null; then
    # gdbserver spawns the child as its own process group leader (setsid),
    # so killing the gdbserver PID leaves the app running. Kill the group.
    kill -TERM -- "-$PID" 2>/dev/null || kill -TERM "$PID"
    sleep 1
    if kill -0 "$PID" 2>/dev/null; then
        kill -KILL -- "-$PID" 2>/dev/null || kill -KILL "$PID"
    fi
    echo "stopped gdbserver + app (pid $PID)"
else
    echo "gdbserver pid $PID already gone"
fi
rm -f "$PID_FILE"