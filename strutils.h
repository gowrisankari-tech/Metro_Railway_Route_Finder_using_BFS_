#ifndef STRUTILS_H
#define STRUTILS_H

/* Case-insensitive string equality check.
   Deliberately implemented by hand with <ctype.h>'s tolower()
   rather than using strcasecmp()/_stricmp(), since those are
   POSIX/MSVC extensions, not ISO C - this keeps the project
   portable across compilers/platforms without relying on a
   non-standard library function. Returns 1 if equal
   (ignoring case), 0 otherwise (including if either pointer
   is NULL). */
int stringsEqualIgnoreCase(const char *a, const char *b);

#endif
