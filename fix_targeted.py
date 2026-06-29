#!/usr/bin/env python3
"""Fix remaining errors with exact substring replacements."""
import re

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp') as f:
    c = f.read()

n = 0

def rep(old, new):
    global n
    if old in c:
        c = c.replace(old, new)
        n += 1
        print(f"  FIXED: {old[:60]}...")
        return True
    else:
        print(f"  MISSING: