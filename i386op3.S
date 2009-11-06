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

/* instructions from 0x80 to 0xBF */

add_a_b:
	ADD_A_R(RB)
	TIME(4)

add_a_c:
	ADD_A_R(RC)
	TIME(4)

add_a_d:
	ADD_A_R(MD)
	TIME(4)

add_a_e:
	ADD_A_R(ME)
	TIME(4)

add_a_h:
	ADD_A_R(RH)
	TIME(4)

add_a_l:
	ADD_A_R(RL)
	TIME(4)

add_a_ihl:
	ADD_A_R((HLP))
	TIME(7)

add_a_a:
	ADD_A_R(RA)
	TIME(4)



adc_a_b:
	ADC_A_R(RB)
	TIME(4)

adc_a_c:
	ADC_A_R(RC)
	TIME(4)

adc_a_d:
	ADC_A_R(MD)
	TIME(4)

adc_a_e:
	ADC_A_R(ME)
	TIME(4)

adc_a_h:
	ADC_A_R(RH)
	TIME(4)

adc_a_l:
	ADC_A_R(RL)
	TIME(4)

adc_a_ihl:
	ADC_A_R((HLP))
	TIME(7)
	
adc_a_a:
	ADC_A_R(RA)
	TIME(4)



sub_b:
	SUB_R(RB)
	TIME(4)

sub_c:
	SUB_R(RC)
	TIME(4)

sub_d:
	SUB_R(MD)
	TIME(4)

sub_e:
	SUB_R(ME)
	TIME(4)

sub_h:
	SUB_R(RH)
	TIME(4)

sub_l:
	SUB_R(RL)
	TIME(4)

sub_ihl:
	SUB_R((HLP))
	TIME(7)

sub_a:
	SUB_R(RA)
	TIME(4)


sbc_a_b:
	SBC_A_R(RB)
	TIME(4)

sbc_a_c:
	SBC_A_R(RC)
	TIME(4)

sbc_a_d:
	SBC_A_R(MD)
	TIME(4)

sbc_a_e:
	SBC_A_R(ME)
	TIME(4)

sbc_a_h:
	SBC_A_R(RH)
	TIME(4)

sbc_a_l:
	SBC_A_R(RL)
	TIME(4)

sbc_a_ihl:
	SBC_A_R((HLP))
	TIME(7)

sbc_a_a:
	SBC_A_R(RA)
	TIME(4)


and_b:
	AND_R(RB)
	TIME(4)

and_c:
	AND_R(RC)
	TIME(4)

and_d:
	AND_R(MD)
	TIME(4)

and_e:
	AND_R(ME)
	TIME(4)

and_h:
	AND_R(RH)
	TIME(4)

and_l:
	AND_R(RL)
	TIME(4)

and_ihl:
	AND_R((HLP))
	TIME(7)

and_a:
	AND_R(RA)
	TIME(4)


xor_b:
	XOR_R(RB)
	TIME(4)

xor_c:
	XOR_R(RC)
	TIME(4)

xor_d:
	XOR_R(MD)
	TIME(4)

xor_e:
	XOR_R(ME)
	TIME(4)

xor_h:
	XOR_R(RH)
	TIME(4)

xor_l:
	XOR_R(RL)
	TIME(4)

xor_ihl:
	XOR_R((HLP))
	TIME(7)

xor_a:
	xorb RA, RA
	andb $~ALLF, RF
	orb $ZPF, RF
	TIME(4)


or_b:
	OR_R(RB)
	TIME(4)

or_c:
	OR_R(RC)
	TIME(4)

or_d:
	OR_R(MD)
	TIME(4)

or_e:
	OR_R(ME)
	TIME(4)

or_h:
	OR_R(RH)
	TIME(4)

or_l:
	OR_R(RL)
	TIME(4)

or_ihl:
	OR_R((HLP))
	TIME(7)

or_a:
	OR_R(RA)
	TIME(4)


cp_b:
	CP_R(RB)
	TIME(4)

cp_c:
	CP_R(RC)
	TIME(4)

cp_d:
	CP_R(MD)
	TIME(4)

cp_e:
	CP_R(ME)
	TIME(4)

cp_h:
	CP_R(RH)
	TIME(4)

cp_l:
	CP_R(RL)
	TIME(4)

cp_ihl:
	CP_R((HLP))
	TIME(7)

cp_a:
	CP_R(RA)
	TIME(4)


#include "i386op3x.S"
