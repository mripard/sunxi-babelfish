/*
 * Original code taken from pxa-impedance-matcher, written by Daniel
 * Mack <zonque@gmail.com>
 */

#ifndef _PRINT_H
#define _PRINT_H

#include "types.h"

void putch(char c);
void printhex(u32 u);
void putstr(const char *s);

#endif /* _PRINT_H */
