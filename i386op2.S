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

/* instructions from 0x40 to 0x7F */

halt:
	movl $1, HALTSTATE
haltstate:
	movl TC, VWP
	testl VWP, VWP
	jz haltstate_tc0
	decl VWP
haltstate_tc0:
	sarl $2, VWP
	incl VWP
	addl VWP, RL7
	leal 0(,VWP,4), VWP
	subl VWP, TC
	jmp loop_end
	ALIGN
	
#define LD_R_R(rd, rs) \
	movb rs, rd		; \
	TIME(4)

#define LD_R_IHL(rd) \
	movb (HLP), rd		; \
	TIME(7)

#define LD_IHL_R(rs) \
	PUTMEM_RHL(ld_ihl_ ## rs, rs, 7)


ld_b_c: 
	LD_R_R(RB, RC)
ld_b_d: 
	LD_R_R(RB, MD)
ld_b_e: 
	LD_R_R(RB, ME)
ld_b_h: 
	LD_R_R(RB, RH)
ld_b_l: 
	LD_R_R(RB, RL)
ld_b_ihl:
	LD_R_IHL(RB)
ld_b_a: 
	LD_R_R(RB, RA)


ld_c_b: 
	LD_R_R(RC, RB)
ld_c_d: 
	LD_R_R(RC, MD)
ld_c_e: 
	LD_R_R(RC, ME)
ld_c_h: 
	LD_R_R(RC, RH)
ld_c_l: 
	LD_R_R(RC, RL)
ld_c_ihl:
	LD_R_IHL(RC)
ld_c_a: 
	LD_R_R(RC, RA)

ld_d_b: 
	LD_R_R(MD, RB)
ld_d_c: 
	LD_R_R(MD, RC)
ld_d_e:
	movb ME, RW
	LD_R_R(MD, RW)
ld_d_h: 
	LD_R_R(MD, RH)
ld_d_l: 
	LD_R_R(MD, RL)
ld_d_ihl:
	movb (HLP), RW
	movb RW, MD
	TIME(7)
ld_d_a: 
	LD_R_R(MD, RA)

	
ld_e_b: 
	LD_R_R(ME, RB)
ld_e_c: 
	LD_R_R(ME, RC)
ld_e_d: 
	movb MD, RW
	LD_R_R(ME, RW)
ld_e_h: 
	LD_R_R(ME, RH)
ld_e_l: 
	LD_R_R(ME, RL)
ld_e_ihl:
	movb (HLP), RW
	movb RW, ME
	TIME(7)
ld_e_a: 
	LD_R_R(ME, RA)


ld_h_b: 
	LD_R_R(RH, RB)
ld_h_c: 
	LD_R_R(RH, RC)
ld_h_d: 
	LD_R_R(RH, MD)
ld_h_e: 
	LD_R_R(RH, ME)
ld_h_l: 
	LD_R_R(RH, RL)
ld_h_ihl:
	LD_R_IHL(RH)
ld_h_a: 
	LD_R_R(RH, RA)


ld_l_b: 
	LD_R_R(RL, RB)
ld_l_c: 
	LD_R_R(RL, RC)
ld_l_d: 
	LD_R_R(RL, MD)
ld_l_e: 
	LD_R_R(RL, ME)
ld_l_h: 
	LD_R_R(RL, RH)
ld_l_ihl:
	LD_R_IHL(RL)
ld_l_a: 
	LD_R_R(RL, RA)


ld_ihl_b: 
	LD_IHL_R(RB)
ld_ihl_c: 
	LD_IHL_R(RC)
ld_ihl_d:
	movb MD, RV
	LD_IHL_R(RV)
ld_ihl_e:
	movb ME, RW
	LD_IHL_R(RW)
ld_ihl_h: 
	LD_IHL_R(RH)
ld_ihl_l: 
	LD_IHL_R(RL)
ld_ihl_a: 
	LD_IHL_R(RA)
	
ld_a_b: 
	LD_R_R(RA, RB)
ld_a_c: 
	LD_R_R(RA, RC)
ld_a_d: 
	LD_R_R(RA, MD)
ld_a_e: 
	LD_R_R(RA, ME)
ld_a_h: 
	LD_R_R(RA, RH)
ld_a_l: 
	LD_R_R(RA, RL)
ld_a_ihl:
	LD_R_IHL(RA)

#include "i386op2x.S"
