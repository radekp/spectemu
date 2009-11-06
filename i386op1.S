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


#define INC(m) \
	incb m			; \
	VCFLAG(SZPF, SZF)       /* Nagy Dani says N and H don't change */
/*	VCFLAG(BUTCF, SZHF) */  

#define DEC(m) \
	decb m			; \
	VCFLAG(SZPF, SZF)       /* Nagy Dani says N and H don't change */
/*	VSFLAG(BUTCF, SZHF) */

#define DADD(h1, l1, h2, l2) \
	addb l2, l1		; \
	adcb h2, h1		; \
	CFLAG(HNCF, HCF)


/* instructions from 0x00 to 0x3F */
		
nop:
	TIME_NC(4)
	
ex_af_afb:
	xchgw BAF, RAF
	TIME_NC(4)
	
djnz_e:
	decb RB
	jne djnz_e_1
	INCPC
	TIME_NC(8)
djnz_e_1:
	FETCHS
	addw RVW, RPC
	TIME(13)
	
jr_e:
	FETCHS
	addw RVW, RPC
	TIME(12)
	
jr_nz_e:
	JNCOND(ZF, jr_e)
	INCPC
	TIME_NC(7)
	
jr_z_e:
	JCOND(ZF, jr_e)
	INCPC
	TIME_NC(7)
	
jr_nc_e:
	JNCOND(CF, jr_e)
	INCPC
	TIME_NC(7)

jr_c_e:
	JCOND(CF, jr_e)
	INCPC
	TIME_NC(7)

ld_bc_nn:
	FETCHR(RC)
	FETCHR(RB)	
	TIME_NC(10)
	
ld_de_nn:
	DFETCH
	movw RVW, MDE
	TIME(10)
	
ld_hl_nn:
	FETCHR(RL)
	FETCHR(RH)
	TIME(10)

ld_sp_nn:
	DFETCH
	movw RVW, RSP
	TIME(10)

add_hl_bc:
	DADD(RH, RL, RB, RC)
	TIME(11)


add_hl_de:
	DADD(RH, RL, MD, ME)
	TIME(11)

add_hl_hl:
	DADD(RH, RL, RH, RL)
	TIME(11)

add_hl_sp:
	movw RSP, RVW
	DADD(RH, RL, RV, RW)
	TIME(11)

inc_bc:
	incw RBC
	TIME_NC(6)
	
inc_de:
	incw MDE
	TIME_NC(6)
	
inc_hl:
	incw RHL
	TIME_NC(6)

inc_sp:
	incw RSP
	TIME_NC(6)

dec_bc:
	decw RBC
	TIME_NC(6)
	
dec_de:
	decw MDE
	TIME_NC(6)
	
dec_hl:
	decw RHL
	TIME_NC(6)

dec_sp:
	decw RSP
	TIME_NC(6)

ld_ibc_a:
	PUTMEM_RBC(ld_ibc_a, RA, 7)
	
ld_ide_a:
	movl MDE, VWP
	PUTMEM_RVW(ld_ide_a, RA, 7)

ld_inn_hl:
	DFETCHP
	PUTMEM_RVW_NE(ld_inn_hl, RL)
	incw RVW
	PUTMEM_RVW(ld_inn_hl, RH, 16)

ld_inn_a:
	DFETCHP
	PUTMEM_RVW(ld_inn_a, RA, 13)
	
ld_a_ibc:
	movb (BCP), RA
	TIME_NC(7)

ld_a_ide:
	movl MDE, VWP
	movb (VWP), RA
	TIME(7)
			
ld_hl_inn:
	DFETCHP
	movb (VWP), RL
	incw RVW
	movb (VWP), RH
	TIME(16)

ld_a_inn:
	DFETCHP
	movb (VWP), RA
	TIME(13)

inc_b:	
	INC(RB)
	TIME(4)
	
inc_c:  
	INC(RC)
	TIME(4)
	
inc_d:	
	INC(MD)
	TIME(4)

inc_e:	
	INC(ME)
	TIME(4)
	
inc_h:	
	INC(RH)
	TIME(4)
		
inc_l:	
	INC(RL)
	TIME(4)

inc_ihl:
	MODMEM(INC, 11)
				
inc_a:	
	INC(RA)
	TIME(4)

		
dec_b:	
	DEC(RB)
	TIME(4)
	
dec_c:  
	DEC(RC)
	TIME(4)

dec_d:	
	DEC(MD)
	TIME(4)
	
dec_e:	
	DEC(ME)
	TIME(4)
	
dec_h:	
	DEC(RH)
	TIME(4)
	
dec_l:	
	DEC(RL)
	TIME(4)

dec_ihl:
	MODMEM(DEC, 11)

dec_a:	
	DEC(RA)
	TIME(4)
	
ld_b_n:
	FETCHR(RB)
	TIME(7)
	
ld_c_n:
	FETCHR(RC)
	TIME(7)
	
ld_d_n:
	FETCH
	movb RW, MD
	TIME(7)
	
ld_e_n:
	FETCH
	movb RW, ME
	TIME(7)
	
ld_h_n:
	FETCHR(RH)
	TIME(7)
	
ld_l_n:
	FETCHR(RL)
	TIME(7)
	
ld_ihl_n:
	FETCH
	PUTMEM_RHL(ld_ihl_n, RW, 10)

ld_a_n:
	FETCHR(RA)
	TIME_NC(7)
	

rlca:
	rolb RA
	CFLAG(HNCF, CF)
	TIME(4)
						
rrca:
	rorb RA
	CFLAG(HNCF, CF)
	TIME(4)
						
rla:
	SFLAGS
	rclb RA
	CFLAG(HNCF, CF)
	TIME(4)
						
rra:
	SFLAGS
	rcrb RA
	CFLAG(HNCF, CF)
	TIME(4)

	
#define DAAFLAGS \
	CFLAG(BUTNF, BUTNF) 
	
daa:
	movb RA, RW
	testb $NF, RF
	jnz daa_s
	SFLAGS
	daa
	movb RW, RA
	DAAFLAGS
	TIME(4)
daa_s:	
	SFLAGS
	das
	movb RW, RA
	DAAFLAGS
	TIME(4)
						
cpl:
	notb RA
	orb $HNF, RF	
	TIME_NC(4)
						
scf:
	andb $~HNF, RF
	orb $CF, RF
	TIME_NC(4)
						
ccf:
	xorb $CF, RF
	andb $~NF, RF
	TIME_NC(4)


#include "i386op1x.S"
