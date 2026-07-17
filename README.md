# ctimer

Simple CLI time tracker written in C using SQLite.

ctimer is a lightweight terminal application for tracking daily activities. 
Instead of scheduling tasks by fixed hours, it records how much time is spent on each activity.
Built for speed and minimal footprint. It runs instantly and consumes virtually zero system resources compared to bloated GUI trackers.

## Features

- Start a task
- Stop the current task
- Automatically close the previous task when starting a new one
- Check the current running task
- View all recorded sessions
- Store data locally using SQLite
- Display readable timestamps and durations

## Usage

```bash
ctimer [OPTION] [TASK]
```

## Options

```
-s, --start <task>          Start a new task.
-p, --stop                  Stop the current task.
-c, --check                 Show all recorded sessions.
-w, --watch                 Show the current task in real time.
-d, --delete <id>           Delete a session by its ID.\n"
-r, --remove-name <task>	Remove all sessions with the same name.\n"
-h, --help                  Display this help message.
```

## Examples

Start a programming session:

```bash
ctimer -s programming
```

Check the current task:

```bash
ctimer
```

Output:

```
programming
Start: 16/07/2026 15:36:04
Duration: 01:42:18
```

Stop the current task:

```bash
ctimer -p
```

View history:

```bash
ctimer -c
```

Output:

```
programming 16/07/2026 15:25:10 -> 16/07/2026 15:25:31
study        16/07/2026 15:36:04 -> --running--
```

## How it works

ctimer stores sessions in a local SQLite database.

Each session contains:

| Field | Description |
|-------|-------------|
| id | Unique session identifier |
| task | Task name |
| start | Unix timestamp when the task started |
| end | Unix timestamp when the task ended |

Only one task can be active at a time. Starting a new task automatically finishes the previous one.

## Building

Requirements:

- CC if you will use the script, or just any C compiler
- SQLite3 development library

Example for requirements:
```bash
sudo apt install libsqlite3-dev
```

Or:
```bash
sudo pacman -S sqlite3
```

Example:

```bash
./scripts/build.sh make
```
I decided to not use Makefile and just use scripts to avoid dependencies.

or manually:

```bash
cc -std=c11 -Wall -Wextra -Wpedantic src/*.c -lsqlite3 -o bin/ctimer
```

## Installation
```bash
chmod +x ./scripts/install.sh
./scripts/install.sh
```

## Database

The database file is created automatically:

```
~/.local/share/ctimer/tracker.db
```

No external configuration is required.

## Future ideas

- Daily statistics
- Time spent per task
- Export reports
- Goals and daily targets

## License

MIT
