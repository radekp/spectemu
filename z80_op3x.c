/* 
 * Copyright (C) 1996-2004 Szeredi Miklos
 * Email: mszeredi@inf.bme.hu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See the file COPYING. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* IX */

ADD_A_R(ixh, XH, 4)
ADD_A_R(ixl, XL, 5)

ADC_A_R(ixh, XH, 4)
ADC_A_R(ixl, XL, 5)

SUB_R(ixh, XH, 4)
SUB_R(ixl, XL, 5)

SBC_A_R(ixh, XH, 4)
SBC_A_R(ixl, XL, 5)

AND_R(ixh, XH, 4)
AND_R(ixl, XL, 5)

XOR_R(ixh, XH, 4)
XOR_R(ixl, XL, 5)

OR_R(ixh, XH, 4)
OR_R(ixl, XL, 5)

CP_R(ixh, XH, 4)
CP_R(ixl, XL, 5)

ARIID(add_a, ADD, 0, ix, XIX)
ARIID(adc_a, ADC, 1, ix, XIX)
ARIID(sub,   SUB, 2, ix, XIX)
ARIID(sbc_a, SBC, 3, ix, XIX)
ARIID(and,   AND, 4, ix, XIX)
ARIID(xor,   XOR, 5, ix, XIX)
ARIID(or,    OR,  6, ix, XIX)
ARIID(cp,    CP,  7, ix, XIX)

/* IY */

ADD_A_R(iyh, YH, 4)
ADD_A_R(iyl, YL, 5)

ADC_A_R(iyh, YH, 4)
ADC_A_R(iyl, YL, 5)

SUB_R(iyh, YH, 4)
SUB_R(iyl, YL, 5)

SBC_A_R(iyh, YH, 4)
SBC_A_R(iyl, YL, 5)

AND_R(iyh, YH, 4)
AND_R(iyl, YL, 5)

XOR_R(iyh, YH, 4)
XOR_R(iyl, YL, 5)

OR_R(iyh, YH, 4)
OR_R(iyl, YL, 5)

CP_R(iyh, YH, 4)
CP_R(iyl, YL, 5)

ARIID(add_a, ADD, 0, iy, XIY)
ARIID(adc_a, ADC, 1, iy, XIY)
ARIID(sub,   SUB, 2, iy, XIY)
ARIID(sbc_a, SBC, 3, iy, XIY)
ARIID(and,   AND, 4, iy, XIY)
ARIID(xor,   XOR, 5, iy, XIY)
ARIID(or,    OR,  6, iy, XIY)
ARIID(cp,    CP,  7, iy, XIY)
