# Metro Railway Route Finder (BFS) - Enhanced Edition

## What's new in this revision

- **Admin**: Register/Login (file-backed, via the `auth/` module) ->
  View Stations, Display Network, Add Connection, Remove
  Connection, Find Shortest Route, Save Network, Load Network,
  Back. On a password mismatch, Admin sees a clear
  "Login Failed: Incorrect Username Or Password." message.
- **User**: no login required - goes straight to Metro / Railway /
  Back to Main Menu. Metro and Railway both now compute a **real
  alternate route** (a second BFS that excludes each edge of the
  shortest path in turn - no graph mutation, no extra malloc/free
  calls) alongside the shortest route, with station counts and
  execution time for both. Railway also shows train timings
  (hours).
- **`auth/` module**: file-backed username/password registration
  and login (`users.dat`), using a fixed-size static store (no heap
  allocation) with full error handling on empty/duplicate/oversized
  input. Now used by **Admin** (Register + Login replaces the old
  hardcoded `admin`/`admin123`); User no longer requires any login.
- **New `fileio/` module**: `saveNetwork()` / `loadNetwork()` read
  and write a human-readable `network.dat`, with error handling on
  every `fopen`/`fscanf`/`fprintf`/`fclose` call.
- **Dynamic station network tagging**: previously, Metro/Railway
  search checked a hardcoded name list, so any station Admin added
  on the fly (e.g. a new state or city) was invisible to search
  even though it existed in the graph. Every station is now tagged
  with a real `NetworkType` (`NETWORK_METRO` / `NETWORK_RAILWAY` /
  `NETWORK_BOTH`) at creation time. **Add Connection** asks which
  network a brand-new station belongs to, and Metro/Railway search
  checks that real tag - so anything Admin adds is searchable
  immediately. `network.dat` persists this tag too (with a
  backward-compatible default for files saved before this field
  existed).
- **Case-insensitive lookups**: Metro/Railway station search
  (`findStation()`) and train timing lookups (`showAvailableTrains()`)
  now match regardless of case - `"CHENNAI"`, `"chennai"`, and
  `"Chennai"` all find the same station. Implemented as a small,
  portable hand-written comparison (`strutils.c`) rather than the
  non-standard `strcasecmp()`.
- **Elaborated hop/interchange output**: instead of a bare
  `Number Of Hops/Interchanges : 3`, the route printout now lists
  each segment by name (`Hop 1 : Chennai -> Vijayawada`, etc.), for
  both the shortest and alternate route.
- **Separate execution timing**: shortest-path search and
  alternate-route search are now timed and printed independently
  (`Shortest Path Execution Time`, `Alternate Path Execution Time`),
  plus a combined total.
- **Timestamped logging**: every `app.log` line now has a real
  `[YYYY-MM-DD HH:MM:SS]` prefix instead of just a severity level.
- **Five issues fixed** from the earlier manual code review (see
  `docs/BUILD_AND_TEST.md` for the table): `removeConnection()` now
  reports accurate success/failure, BFS respects the `active` flag
  on removed stations, `addConnection()` rolls back cleanly on
  partial allocation failure, `validation.c`'s functions are wired
  in and actually called, and every `scanf()` return value is
  checked via a shared `readMenuChoice()` helper (which also fixes
  an EOF-handling edge case found during testing).
- **Memory**: `memory.c` now tracks peak live allocations, not just
  totals. Every test run during development showed allocations and
  frees exactly balanced (e.g. 193/193, 95/95) with 0 outstanding
  blocks.
- **CUnit test suite** under `tests/` covering `graph.c`, `queue.c`,
  `bfs.c`, `auth.c`, `strutils.c`, and `train.c` (6 suites total),
  including regression tests for every fixed bug.

## Important honesty note

`cppcheck`, `valgrind`, and `CUnit` were not installed in the
sandbox this was built in, and that sandbox has no network access
to install them. Everything above was verified through manual code
review and the program's own build-in allocation counters instead
of the real tools. **Please run the real tools yourself** using the
exact commands in `docs/BUILD_AND_TEST.md` - I'd like to know if
they find anything the manual review missed. That doc also has a
**section mapping this project against your POC evaluation rubric**,
including two genuine gaps flagged honestly (Code Optimization
measurement, and Multithreading/Helgrind - the app is currently
single-threaded).

## Build

```bash
gcc -Wall -Wextra -g main.c \
    graph/graph.c bfs/bfs.c queue/queue.c train/train.c \
    user/user.c utils/utils.c validation/validation.c \
    logger/logger.c memory/memory.c admin/admin.c \
    auth/auth.c fileio/fileio.c strutils/strutils.c \
    -o metro_railway_finder
./metro_railway_finder
```

## Directory guide

| Folder | Contents |
|---|---|
| `admin/`, `user/`, `auth/` | Menu flows and login/registration |
| `graph/`, `bfs/`, `queue/` | Core data structure + BFS engine |
| `strutils/` | Portable case-insensitive string comparison |
| `fileio/` | Save/Load Network |
| `train/` | Train timing lookups (Railway) |
| `utils/`, `validation/` | Shared input handling + validation |
| `logger/`, `memory/` | Logging (`app.log`) and allocation tracking |
| `tests/` | CUnit test suite + build instructions |
| `docs/` | `BUILD_AND_TEST.md` (cppcheck/valgrind/CUnit commands) and `MISRA_NOTES.md` (honest compliance notes) |

See `docs/BUILD_AND_TEST.md` for full verification steps.
