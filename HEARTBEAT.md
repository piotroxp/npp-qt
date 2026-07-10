# HEARTBEAT.md

Keep empty to skip heartbeat checks. Add tasks when you want periodic checks.

## Fleet Projects (app / code work)

When creating or updating an app in Fleet Control, use a **Fleet Project** folder so files appear under **Projects** and **Project files** in the UI:

- Prefer: create **New project** in Chat → Projects (per agent), then write only under `.openclaw/fleet-projects/<slug>/`
- Do not scatter MVP code in the agent workspace root (main heartbeat will not show those files under Session output files)
