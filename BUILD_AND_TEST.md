# Build & Verification Guide

None of `cppcheck`, `valgrind`, or `CUnit` were available in the
sandbox this project was built in (not pre-installed, and outbound
network access to install them was blocked - `apt-get install`
returned `403 Forbidden`). Everything below was verified a
different way: manual code review for cppcheck-equivalent findings,
and the program's own built-in allocation/free counters
(`memory.c`) for leak-checking, confirmed clean across every test
run performed (0 outstanding blocks each time). Please run the real
tools yourself with the commands below - they'll either confirm the
same result or catch something the manual review missed, and I can
help fix anything they report.

## 1. Build

```bash
gcc -Wall -Wextra -g main.c \
    graph/graph.c bfs/bfs.c queue/queue.c train/train.c \
    user/user.c utils/utils.c validation/validation.c \
    logger/logger.c memory/memory.c admin/admin.c \
    auth/auth.c fileio/fileio.c strutils/strutils.c \
    -o metro_railway_finder
```

Builds warning-free under `-Wall -Wextra` as of this revision.

## 2. cppcheck

```bash
cppcheck --enable=all --suppress=missingIncludeSystem \
    --error-exitcode=1 --xml --xml-version=2 . 2> cppcheck_report.xml
cat cppcheck_report.xml
```

Or for a quick human-readable pass instead of XML:

```bash
cppcheck --enable=all --suppress=missingIncludeSystem .
```

The five issues found in the manual review before this revision
(see the earlier `Cppcheck_Static_Review_Report.docx`) are fixed
here:

| Old finding | Fix in this revision |
|---|---|
| `removeConnection()` always printed success | Now returns `1`/`0` and prints the accurate message (`graph.c`) |
| `BFS()` ignored the `active` flag | `bfsCore()` now checks `isActiveStation()` before visiting any neighbor (`bfs.c`) |
| `addConnection()` left a partial edge on the second `malloc()` failure | First edge is now rolled back with `free()` before returning (`graph.c`) |
| `validation.c` functions were dead code | `validateMenuChoice()` is called from every menu via `utils.c: readMenuChoice()`; `validateStationName()` is called from `admin.c`'s Add Connection flow |
| `scanf()` return values were unchecked | `readMenuChoice()` in `utils.c` checks it everywhere; all menu loops now go through this one function |
| Station/train lookups were case-sensitive (`"CHENNAI"` didn't match `"Chennai"`) | New `strutils.c: stringsEqualIgnoreCase()` (hand-written, no non-standard `strcasecmp()` dependency) is now used by `graph.c: findStation()` and `train.c: isRoute()` |

## 3. valgrind

Since the program is interactive, feed it a scripted session so it
runs to completion and exits cleanly:

```bash
printf "2\n1\nuser1\npass1\n2\nuser1\npass1\n1\n2\nCentral Metro\nAirport\n3\n3\n3\n3\n" \
  | valgrind --leak-check=full --show-leak-kinds=all \
    --log-file=valgrind_report.txt ./metro_railway_finder
cat valgrind_report.txt
```

Look for:

```
All heap blocks were freed -- no leaks are possible
```

The program's own exit-time report (option 3 from the main menu)
also prints allocation/free/peak counters independently of
valgrind:

```
MEMORY STATISTICS
Total Allocations   : ...
Total Frees         : ...
Outstanding Blocks  : 0 (clean - no leaks)
Peak Live Blocks    : ...
```

In every manual test run during development, these two numbers
matched exactly (0 outstanding blocks) - see the "Total
Allocations"/"Total Frees" lines for several representative runs in
this project's chat history.

## 4. CUnit

See `tests/README.md` for the full instructions. Short version:

```bash
sudo apt-get install libcunit1 libcunit1-dev
gcc -Wall -Wextra -g -o run_tests \
    tests/test_runner.c tests/test_graph.c tests/test_queue.c \
    tests/test_bfs.c tests/test_auth.c tests/test_strutils.c \
    tests/test_train.c \
    graph/graph.c bfs/bfs.c queue/queue.c auth/auth.c \
    logger/logger.c memory/memory.c strutils/strutils.c \
    -lcunit
./run_tests
```

## 5. MISRA

See `docs/MISRA_NOTES.md` for exactly what was and wasn't applied -
please read it before assuming full compliance; it's written to be
honest about the gaps, not to oversell the result.

## 6. Coverage against the POC evaluation rubric

Mapping this project's current state to each rubric line, honestly
- items marked (missing) genuinely aren't implemented yet rather
  than glossed over:

| # | Module | Status |
|---|---|---|
| 1 | Scrum Training & Product Backlog | Not part of this codebase - process/planning artifact, not code |
| 2 | POC Discussion (PPT) | Not part of this codebase |
| 3 | Memory Management | `memory.c` tracks allocations/frees/peak; every manual test run showed 0 outstanding blocks. **Run real `valgrind`** (Section 3) for the authoritative check |
| 4 | Code Optimization (O1/O2/O3, `.s` size) | **(missing)** - not yet measured. See note below |
| 5 | Secure Coding & MISRA C | Partially applied - see `docs/MISRA_NOTES.md` for specifics and honest gaps |
| 6 | Test case review (CUnit) | `tests/` has 6 suites covering graph, queue, BFS, auth, strutils, train - including edge cases (NULL args, empty strings, unreachable stations, removed/inactive stations, duplicate registration) |
| 7 | POC HL & LL Design, UML Diagrams | Delivered earlier in this conversation as SVG diagrams |
| 9 | Code Analysis & Coverage (cppcheck) | Manual review done (Section 2); **run real `cppcheck`** for the authoritative check. Code coverage (e.g. `gcov`) has not been measured |
| 10 | Multithreading (Helgrind) | **(missing)** - the program is currently single-threaded; there is nothing for Helgrind to check yet. See note below |
| 11 | Timestamping & Logging | `logger.c` now prefixes every line with a real `YYYY-MM-DD HH:MM:SS` timestamp; **run cppcheck** to confirm no warnings on the new `time.h` usage |
| 12 | Coding, Compiling & Debugging (GDB, Make) | Compiles clean; no `Makefile` exists yet (build command is passed to `gcc` directly - see Section 1); GDB fault-injection walkthroughs haven't been done |

### Two genuine gaps worth flagging explicitly

- **Code Optimization (#4):** to measure this, compile the same
  source at each level and compare assembly size:
  ```bash
  gcc -O1 -S graph/graph.c -o graph_O1.s
  gcc -O2 -S graph/graph.c -o graph_O2.s
  gcc -O3 -S graph/graph.c -o graph_O3.s
  wc -l graph_O1.s graph_O2.s graph_O3.s
  ```
  This hasn't been run/recorded yet - happy to add a documented
  comparison across all `.c` files if useful.
- **Multithreading (#10):** nothing in this codebase currently uses
  threads, so Helgrind has nothing to report on for thread
  creation/sync/races/deadlocks. If this line item needs to be
  satisfied, a reasonable, low-risk addition would be a
  mutex-protected `logger.c` (multiple threads could safely log
  concurrently) plus one clearly-scoped worker thread (e.g. a
  background autosave that periodically calls `saveNetwork()`).
  This wasn't added speculatively since it changes the program's
  concurrency model - let me know if you want it and I'll implement
  it carefully rather than guess at what "enough" multithreading
  looks like for this rubric.
- **`Makefile` (#12):** not created yet; the build command in
  Section 1 works today, but a `Makefile` would match the rubric's
  "Tool(GDB, Make)" line more directly if that's expected as a
  literal artifact.
