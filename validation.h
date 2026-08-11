#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdint.h>

/* Generalized from a hardcoded 1-6 range so every menu in the
   project (Admin: 8 options, User: 3, Metro: 3, Railway: 2)
   can reuse the same validator instead of duplicating range
   checks - this is also what makes the function actually
   get called (previously defined but unused, per prior code
   review finding F4). */
uint8_t validateMenuChoice(int choice, int minChoice, int maxChoice);

uint8_t validateStationName(const char *name);

uint8_t validateStationIndex(int index);

#endif
