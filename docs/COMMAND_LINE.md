# Command-Line Reference

Complete reference for Notepad--Qt command-line arguments, environment variables, and IPC.

---

## Usage



---

## Options

### , 
Show the built-in help text and exit.

### , 
Open all files in read-only mode. The UI disables Save () and all modifications are discarded on close.

### , 
Always open a new application instance, even if an existing instance is running. By default, Notepad--Qt uses a singleton lock and sends files to the running instance.

### 
Load the application without loading any plugins. Useful for diagnosing plugin-related crashes:


### , 
Use  as the configuration directory instead of .


### , 
Override the default plugin directory ().


### , 
Load a session file on startup. The session JSON must conform to the format in [docs/ARCHITECTURE.md](ARCHITECTURE.md#session-persistence).


### 
Apply a named theme profile at startup.  must match a file in  (without the  extension).


---

## Environment Variables

### 
Selects the Qt platform plugin. Must be set **before**  construction.

| Value | Platform |
|-------|----------|
|  | Headless (no display) |
|  | X11 (default on Linux) |
|  | Wayland |
|  | Windows |



### 
Overrides the Qt platform theme (e.g., GTK, Fusion, Breeze).



### 
Used by Qt to connect to the X11 server. Required for GUI mode on a remote machine:


### 
Overrides the base XDG config directory. Notepad--Qt stores config under :


---

## Exit Codes

| Code | Meaning |
|------|---------|
|  | Normal exit |
|  | Error (file not found, bad arguments, init failure) |
|  (SIGABRT) | Unhandled exception (ASAN builds catch this) |
|  (SIGSEGV) | Crash |

Exit code is useful in scripts:


---

## Session File Format

The session JSON format ( / ):



All fields are optional. Missing fields use defaults (empty geometry → centre on screen; missing panels → hidden).
