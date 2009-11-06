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
	
#define LD_R_IIXD(rd) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIX, RVW		; \
	movb (VWP), rd

#define LD_IIXD_R(rs) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIX, RVW		; \
	PUTMEM_RVW(ld_iixd_ ## rs, rs, 15)

#define LD_IIXD_M(rs) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIX, RVW		; \
	pushl HLP		; \
	movb rs, RL		; \
	PUTMEM_RVW_NE(ld_iixd_ ## rs, RL); \
	popl HLP		; \
	TIMEX(19)

ld_b_iixd:
	LD_R_IIXD(RB)
	TIMEX(19)

ld_b_ixh: 
	movb MIXH, RB
	TIMEX(8)

ld_b_ixl: 
	movb MIXL, RB
	TIMEX(8)


ld_c_iixd:
	LD_R_IIXD(RC)
	TIMEX(19)

ld_c_ixh: 
	movb MIXH, RC
	TIMEX(8)

ld_c_ixl: 
	movb MIXL, RC
	TIMEX(8)

ld_d_iixd:
	LD_R_IIXD(RW)
	movb RW, MD
	TIMEX(19)

ld_d_ixh: 
	movb MIXH, RW
	movb RW, MD
	TIMEX(8)

ld_d_ixl: 
	movb MIXL, RW
	movb RW, MD
	TIMEX(8)

ld_e_iixd:
	LD_R_IIXD(RW)
	movb RW, ME
	TIMEX(19)

ld_e_ixh: 
	movb MIXH, RW
	movb RW, ME
	TIMEX(8)

ld_e_ixl: 
	movb MIXL, RW
	movb RW, ME
	TIMEX(8)


ld_h_iixd:
	LD_R_IIXD(RH)
	TIMEX(19)

ld_ixh_b:
	movb RB, MIXH 
	TIMEX(8)
ld_ixh_c: 
	movb RC, MIXH
	TIMEX(8)
ld_ixh_d: 
	movb MD, RW
	movb RW, MIXH
	TIMEX(8)
ld_ixh_e: 
	movb ME, RW
	movb RW, MIXH
	TIMEX(8)
ld_ixh_ixl: 
	movb MIXL, RW
	movb RW, MIXH
	TIMEX(8)
ld_ixh_a: 
	movb RA, MIXH
	TIMEX(8)

ld_l_iixd:
	LD_R_IIXD(RL)
	TIMEX(19)

ld_ixl_b:
	movb RB, MIXL 
	TIMEX(8)
ld_ixl_c: 
	movb RC, MIXL
	TIMEX(8)
ld_ixl_d: 
	movb MD, RW
	movb RW, MIXL
	TIMEX(8)
ld_ixl_e: 
	movb ME, RW
	movb RW, MIXL
	TIMEX(8)
ld_ixl_ixh: 
	movb MIXH, RW
	movb RW, MIXL
	TIMEX(8)
ld_ixl_a: 
	movb RA, MIXL
	TIMEX(8)

ld_iixd_b: 
	LD_IIXD_R(RB)
ld_iixd_c: 
	LD_IIXD_R(RC)
ld_iixd_d:
	LD_IIXD_M(MD)
ld_iixd_e:
	LD_IIXD_M(ME)
ld_iixd_h: 
	LD_IIXD_R(RH)
ld_iixd_l: 
	LD_IIXD_R(RL)
ld_iixd_a: 
	LD_IIXD_R(RA)

ld_a_iixd:
	LD_R_IIXD(RA)
	TIMEX(19)

ld_a_ixh: 
	movb MIXH, RA
	TIMEX(8)

ld_a_ixl: 
	movb MIXL, RA
	TIMEX(8)

/* IY */
	
#define LD_R_IIYD(rd) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIY, RVW		; \
	movb (VWP), rd

#define LD_IIYD_R(rs) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIY, RVW		; \
	PUTMEM_RVW(ld_iiyd_ ## rs, rs, 15)

#define LD_IIYD_M(rs) \
	movl HLP, VWP		; \
	FETCHS			; \
	addw MIY, RVW		; \
	pushl HLP		; \
	movb rs, RL		; \
	PUTMEM_RVW_NE(ld_iiyd_ ## rs, RL); \
	popl HLP		; \
	TIMEX(19)

ld_b_iiyd:
	LD_R_IIYD(RB)
	TIMEX(19)

ld_b_iyh: 
	movb MIYH, RB
	TIMEX(8)

ld_b_iyl: 
	movb MIYL, RB
	TIMEX(8)


ld_c_iiyd:
	LD_R_IIYD(RC)
	TIMEX(19)

ld_c_iyh: 
	movb MIYH, RC
	TIMEX(8)

ld_c_iyl: 
	movb MIYL, RC
	TIMEX(8)

ld_d_iiyd:
	LD_R_IIYD(RW)
	movb RW, MD
	TIMEX(19)

ld_d_iyh: 
	movb MIYH, RW
	movb RW, MD
	TIMEX(8)

ld_d_iyl: 
	movb MIYL, RW
	movb RW, MD
	TIMEX(8)

ld_e_iiyd:
	LD_R_IIYD(RW)
	movb RW, ME
	TIMEX(19)

ld_e_iyh: 
	movb MIYH, RW
	movb RW, ME
	TIMEX(8)

ld_e_iyl: 
	movb MIYL, RW
	movb RW, ME
	TIMEX(8)


ld_h_iiyd:
	LD_R_IIYD(RH)
	TIMEX(19)

ld_iyh_b:
	movb RB, MIYH 
	TIMEX(8)
ld_iyh_c: 
	movb RC, MIYH
	TIMEX(8)
ld_iyh_d: 
	movb MD, RW
	movb RW, MIYH
	TIMEX(8)
ld_iyh_e: 
	movb ME, RW
	movb RW, MIYH
	TIMEX(8)
ld_iyh_iyl: 
	movb MIYL, RW
	movb RW, MIYH
	TIMEX(8)
ld_iyh_a: 
	movb RA, MIYH
	TIMEX(8)

ld_l_iiyd:
	LD_R_IIYD(RL)
	TIMEX(19)

ld_iyl_b:
	movb RB, MIYL 
	TIMEX(8)
ld_iyl_c: 
	movb RC, MIYL
	TIMEX(8)
ld_iyl_d: 
	movb MD, RW
	movb RW, MIYL
	TIMEX(8)
ld_iyl_e: 
	movb ME, RW
	movb RW, MIYL
	TIMEX(8)
ld_iyl_iyh: 
	movb MIYH, RW
	movb RW, MIYL
	TIMEX(8)
ld_iyl_a: 
	movb RA, MIYL
	TIMEX(8)

ld_iiyd_b: 
	LD_IIYD_R(RB)
ld_iiyd_c: 
	LD_IIYD_R(RC)
ld_iiyd_d:
	LD_IIYD_M(MD)
ld_iiyd_e:
	LD_IIYD_M(ME)
ld_iiyd_h: 
	LD_IIYD_R(RH)
ld_iiyd_l: 
	LD_IIYD_R(RL)
ld_iiyd_a: 
	LD_IIYD_R(RA)

ld_a_iiyd:
	LD_R_IIYD(RA)
	TIMEX(19)

ld_a_iyh: 
	movb MIYH, RA
	TIMEX(8)

ld_a_iyl: 
	movb MIYL, RA
	TIMEX(8)

