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
	
ld_ix_nn:
	DFETCH
	movw RVW, MIX
	TIMEX(14)

add_ix_bc:
	xchgw RHL, MIX
	DADD(RH, RL, RB, RC)
	xchgw RHL, MIX
	TIMEX(15)

add_ix_de:
	xchgw RHL, MIX
	DADD(RH, RL, MD, ME)
	xchgw RHL, MIX
	TIMEX(15)

add_ix_ix:
	xchgw RHL, MIX
	DADD(RH, RL, RH, RL)
	xchgw RHL, MIX
	TIMEX(15)

add_ix_sp:
	movw RSP, RVW
	xchgw RHL, MIX
	DADD(RH, RL, RV, RW)
	xchgw RHL, MIX
	TIMEX(15)

inc_ix:
	incw MIX
	TIMEX(10)
	
dec_ix:
	decw MIX
	TIMEX(10)

ld_inn_ix:
	DFETCHP
	xchgw RHL, MIX
	PUTMEM_RVW_NE(ld_inn_ix1, RL)
	incw RVW
	PUTMEM_RVW_NE(ld_inn_ix2, RH)
	xchgw RHL, MIX
	TIMEX(20)

ld_ix_inn:
	DFETCHP
	xchgw RHL, MIX
	movb (VWP), RL
	incw RVW
	movb (VWP), RH
	xchgw RHL, MIX
	TIMEX(20)

inc_iixd:
	IXDGET
	MODMEMID(ix, INC, 23)
	
inc_ixh:	
	INC(MIXH)
	TIMEX(8)
	
inc_ixl:	
	INC(MIXL)
	TIMEX(8)

dec_iixd:
	IXDGET
	MODMEMID(ix, DEC, 23)

dec_ixh:	
	DEC(MIXH)
	TIMEX(8)
	
dec_ixl:	
	DEC(MIXL)
	TIMEX(8)

	
ld_iixd_n:
	IXDGET
	FETCH
	PUTMEMID_RHL(ld_iixd_n, RW, 19)

ld_ixh_n:
	FETCH
	movb RW, MIXH
	TIMEX(11)

ld_ixl_n:
	FETCH
	movb RW, MIXL
	TIMEX(11)
	



/* IY */

ld_iy_nn:
	DFETCH
	movw RVW, MIY
	TIMEX(14)

add_iy_bc:
	xchgw RHL, MIY
	DADD(RH, RL, RB, RC)
	xchgw RHL, MIY
	TIMEX(15)

add_iy_de:
	xchgw RHL, MIY
	DADD(RH, RL, MD, ME)
	xchgw RHL, MIY
	TIMEX(15)

add_iy_iy:
	xchgw RHL, MIY
	DADD(RH, RL, RH, RL)
	xchgw RHL, MIY
	TIMEX(15)

add_iy_sp:
	movw RSP, RVW
	xchgw RHL, MIY
	DADD(RH, RL, RV, RW)
	xchgw RHL, MIY
	TIMEX(15)

inc_iy:
	incw MIY
	TIMEX(10)
	
dec_iy:
	decw MIY
	TIMEX(10)

ld_inn_iy:
	DFETCHP
	xchgw RHL, MIY
	PUTMEM_RVW_NE(ld_inn_iy1, RL)
	incw RVW
	PUTMEM_RVW_NE(ld_inn_iy2, RH)
	xchgw RHL, MIY
	TIMEX(20)

ld_iy_inn:
	DFETCHP
	xchgw RHL, MIY
	movb (VWP), RL
	incw RVW
	movb (VWP), RH
	xchgw RHL, MIY
	TIMEX(20)

inc_iiyd:
	IYDGET
	MODMEMID(iy, INC, 23)
	
inc_iyh:	
	INC(MIYH)
	TIMEX(8)
	
inc_iyl:	
	INC(MIYL)
	TIMEX(8)

dec_iiyd:
	IYDGET
	MODMEMID(iy, DEC, 23)

dec_iyh:	
	DEC(MIYH)
	TIMEX(8)
	
dec_iyl:	
	DEC(MIYL)
	TIMEX(8)

	
ld_iiyd_n:
	IYDGET
	FETCH
	PUTMEMID_RHL(ld_iiyd_n, RW, 19)

ld_iyh_n:
	FETCH
	movb RW, MIYH
	TIMEX(11)

ld_iyl_n:
	FETCH
	movb RW, MIYL
	TIMEX(11)
	
