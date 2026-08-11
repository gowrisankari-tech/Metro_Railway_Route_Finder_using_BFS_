# MISRA C Coding Guideline Notes

This project follows a meaningful subset of MISRA C:2012 practices
appropriate for an educational/demo application. It is **not**
formally MISRA-certified — that requires a licensed static analyzer
(e.g. PC-lint, Polyspace, or a commercial MISRA checker) producing a
compliance matrix, which was not available in the environment this
project was built in. Below is an honest list of what was applied
and what still deviates, so you know exactly where things stand
rather than taking a blanket "MISRA compliant" claim at face value.

## Applied in this revision

- **Rule 10.x (essential type rules):** removed signed/unsigned
  comparison warnings (`bfs.c`, `validation.c`) by casting
  `MAX_STATIONS` to `int` at comparison sites instead of comparing
  `int` against `unsigned int` directly.
- **Rule 17.7 (return value must be used or explicitly discarded):**
  every `scanf()` call site now checks its return value
  (`utils.c: readMenuChoice()`), and `admin.c` uses `(void)` casts
  on `saveNetwork()`/`loadNetwork()` return values where the result
  is only used for the printed message, not control flow.
- **Directive 4.7 (check return values of error-prone functions):**
  every `malloc()`, `fopen()`, `fclose()`, and `fprintf()` call
  added in this revision (`graph.c`, `fileio.c`, `auth.c`) checks
  its return value and logs/reports on failure.
- **Rule 15.5 (single point of exit, encouraged):** partially
  applied — new functions like `readMenuChoice()` and
  `findShortestAndAlternate()` use early returns only for
  guard/error conditions at the top, not scattered through the
  main logic.
- **Rule 17.2 (no recursion):** `printPath()` was recursive in the
  original code; it's been replaced with `printPathArray()`, an
  iterative version that prints from a pre-built path array. BFS
  itself was already iterative.
- **Directive 4.12 avoidance:** the new `auth.c` user store
  deliberately uses a **fixed-size static array**
  (`AUTH_MAX_USERS = 100`) instead of dynamic allocation, since a
  bounded static structure was sufficient and avoids adding more
  heap traffic.
- **No magic numbers for buffer sizes:** `AUTH_FIELD_LEN`,
  `NAME_LEN`, `MAX_STATIONS` are named constants used consistently
  instead of repeated literals.

## Known, deliberate deviations (not fixed - documented instead)

- **Rule 21.3 (no dynamic memory - `malloc`/`free`):** `graph.c`
  still uses `malloc()` for adjacency-list edges. Eliminating this
  would mean converting the adjacency list to a fixed-size
  adjacency matrix or a static edge pool sized to
  `MAX_STATIONS * MAX_STATIONS`, which is a larger structural
  change than this revision covers. Flagging it here rather than
  silently leaving it unmentioned.
- **Rule 8.7 (functions used in one file should be static):** most
  internal helper functions across modules use file-local `static`
  correctly (e.g. `metroMenu()`, `railwayMenu()`,
  `usernameExists()`), but this wasn't re-audited file-by-file for
  every function in the pre-existing code (e.g. `train.c`'s
  `isRoute()` is already `static` and correct; this note is about
  the general practice, not a specific known violation).
- **Rule 8.13 (pointers should be `const` where possible):** applied
  in the new code (`isActiveStation(const Graph *graph, ...)`,
  `printPathArray(const int path[], ...)`) but not retrofitted
  across every existing function signature in `graph.c`/`bfs.c`
  that could theoretically take a `const Graph *`.
- **No formal MISRA compliance matrix:** the claim here is "applied
  MISRA-influenced practices where practical," not "certified
  MISRA compliant." A real compliance sign-off needs a licensed
  static analyzer's deviation report.

## Recommended next step

If MISRA compliance is a hard requirement (e.g. for an academic
rubric or an actual embedded deliverable), run a real MISRA checker
against this source and treat this document as a starting point,
not the final word. Cppcheck's `--addon=misra` (if you have the
MISRA addon script) is a reasonable free approximation:

```bash
cppcheck --addon=misra --enable=all .
```
