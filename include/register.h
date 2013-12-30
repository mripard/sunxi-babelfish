/*
 * Original code taken from pxa-impedance-matcher, written by Jason
 * Cooper <jason@lakedaemon.net>
 */

#ifndef _REGISTER_H
#define _REGISTER_H

#include <types.h>

#define SID_BASE	0x01c23800

void writel(u32, u32);
u32 readl(u32);

#define BIT(nr)         (1UL << (nr))

#endif
