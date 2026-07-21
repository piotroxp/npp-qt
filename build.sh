#!/bin/bash
cd /home/node/.openclaw/workspace
cmake --build build2/ 2>&1 | tee /tmp/build_output.txt | grep -E "error:|Built target" | head -20
