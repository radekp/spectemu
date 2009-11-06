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

/* -*- asm -*- */

/* IX */

add_a_iixd:
	IXDGET
	ADD_A_R((HLP))
	RSHL
	TIMEX(19)

add_a_ixh:
	ADD_A_R(MIXH)
	TIMEX(8)

add_a_ixl:
	ADD_A_R(MIXL)
	TIMEX(8)

adc_a_iixd:
	IXDGET
	ADC_A_R((HLP))
	RSHL
	TIMEX(19)

adc_a_ixh:
	ADC_A_R(MIXH)
	TIMEX(8)

adc_a_ixl:
	ADC_A_R(MIXL)
	TIMEX(8)

sub_iixd:
	IXDGET
	SUB_R((HLP))
	RSHL
	TIMEX(19)

sub_ixh:
	SUB_R(MIXH)
	TIMEX(8)

sub_ixl:
	SUB_R(MIXL)
	TIMEX(8)


sbc_a_iixd:
	IXDGET
	SBC_A_R((HLP))
	RSHL
	TIMEX(19)

sbc_a_ixh:
	SBC_A_R(MIXH)
	TIMEX(8)

sbc_a_ixl:
	SBC_A_R(MIXL)
	TIMEX(8)


and_iixd:
	IXDGET
	AND_R((HLP))
	RSHL
	TIMEX(19)

and_ixh:
	AND_R(MIXH)
	TIMEX(8)

and_ixl:
	AND_R(MIXL)
	TIMEX(8)

xor_iixd:
	IXDGET
	XOR_R((HLP))
	RSHL
	TIMEX(19)

xor_ixh:
	XOR_R(MIXH)
	TIMEX(8)

xor_ixl:
	XOR_R(MIXL)
	TIMEX(8)

or_iixd:
	IXDGET
	OR_R((HLP))
	RSHL
	TIMEX(19)

or_ixh:
	OR_R(MIXH)
	TIMEX(8)

or_ixl:
	OR_R(MIXL)
	TIMEX(8)

cp_iixd:
	IXDGET
	CP_R((HLP))
	RSHL
	TIMEX(19)

cp_ixh:
	CP_R(MIXH)
	TIMEX(8)

cp_ixl:
	CP_R(MIXL)
	TIMEX(8)


/* IY */

add_a_iiyd:
	IYDGET
	ADD_A_R((HLP))
	RSHL
	TIMEX(19)

add_a_iyh:
	ADD_A_R(MIYH)
	TIMEX(8)

add_a_iyl:
	ADD_A_R(MIYL)
	TIMEX(8)

adc_a_iiyd:
	IYDGET
	ADC_A_R((HLP))
	RSHL
	TIMEX(19)

adc_a_iyh:
	ADC_A_R(MIYH)
	TIMEX(8)

adc_a_iyl:
	ADC_A_R(MIYL)
	TIMEX(8)

sub_iiyd:
	IYDGET
	SUB_R((HLP))
	RSHL
	TIMEX(19)

sub_iyh:
	SUB_R(MIYH)
	TIMEX(8)

sub_iyl:
	SUB_R(MIYL)
	TIMEX(8)


sbc_a_iiyd:
	IYDGET
	SBC_A_R((HLP))
	RSHL
	TIMEX(19)

sbc_a_iyh:
	SBC_A_R(MIYH)
	TIMEX(8)

sbc_a_iyl:
	SBC_A_R(MIYL)
	TIMEX(8)


and_iiyd:
	IYDGET
	AND_R((HLP))
	RSHL
	TIMEX(19)

and_iyh:
	AND_R(MIYH)
	TIMEX(8)

and_iyl:
	AND_R(MIYL)
	TIMEX(8)

xor_iiyd:
	IYDGET
	XOR_R((HLP))
	RSHL
	TIMEX(19)

xor_iyh:
	XOR_R(MIYH)
	TIMEX(8)

xor_iyl:
	XOR_R(MIYL)
	TIMEX(8)

or_iiyd:
	IYDGET
	OR_R((HLP))
	RSHL
	TIMEX(19)

or_iyh:
	OR_R(MIYH)
	TIMEX(8)

or_iyl:
	OR_R(MIYL)
	TIMEX(8)

cp_iiyd:
	IYDGET
	CP_R((HLP))
	RSHL
	TIMEX(19)

cp_iyh:
	CP_R(MIYH)
	TIMEX(8)

cp_iyl:
	CP_R(MIYL)
	TIMEX(8)


