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

/* instructions prefixed by CB */


#define BITFL \
	lahf			; \
	andb $SF, RW		; \
	andb $~SZNF, RF		; \
	andb $ZF, RV		; \
	orb RV, RF		; \
	orb RW, RF

#define ROTSHFL \
	CFLAG(ALLF, CF)		; \
	testb RW,RW		; \
	CFLAG(SZPF, SZPF)
	
	
#ifdef FASTFLAGS

#define BITFLAGS BITFL
#define ROTSHFLAGS ROTSHFL
	
#else

	ALIGN
bitflags:	
	BITFL
	ret
	ALIGN
	
rotshflags:
	ROTSHFL
	ret
	ALIGN
	
#define BITFLAGS \
	call bitflags

#define ROTSHFLAGS \
	call rotshflags
	
#endif	

	
#define RLC_R(r) \
	rolb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define RRC_R(r) \
	rorb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define RL_R(r) \
	SFLAGS			; \
	rclb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define RR_R(r) \
	SFLAGS			; \
	rcrb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define SLA_R(r) \
	salb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define SRA_R(r) \
	sarb r			; \
	movb r, RW		; \
	ROTSHFLAGS

#define SLL_R(r) \
	shlb r			; \
	CFLAG(ALLF, CF)		; \
	orb $1, r		; \
	CFLAG(SZPF, SZPF)
	
#define SRL_R(r) \
	shrb r			; \
	movb r, RW		; \
	ROTSHFLAGS


#define BIT(n, r) \
	movb r, RW		; \
	testb $(1 << n), RW	; \
	BITFLAGS

#define SET(n, r) \
	orb $(1 << n), r

#define RES(n, r) \
	andb $~(1 << n), r



rlc_b:
	RLC_R(RB)
	TIME(8)

rlc_c:
	RLC_R(RC)
	TIME(8)

rlc_d:
	RLC_R(MD)
	TIME(8)

rlc_e:
	RLC_R(ME)
	TIME(8)

rlc_h:
	RLC_R(RH)
	TIME(8)

rlc_l:
	RLC_R(RL)
	TIME(8)

rlc_ihl:
	MODMEM(RLC_R, 15)

rlc_id:
	MODMEMID(id, RLC_R, 23)

rlc_a:
	RLC_R(RA)
	TIME(8)


rrc_b:
	RRC_R(RB)
	TIME(8)

rrc_c:
	RRC_R(RC)
	TIME(8)

rrc_d:
	RRC_R(MD)
	TIME(8)

rrc_e:
	RRC_R(ME)
	TIME(8)

rrc_h:
	RRC_R(RH)
	TIME(8)

rrc_l:
	RRC_R(RL)
	TIME(8)

rrc_ihl:
	MODMEM(RRC_R, 15)

rrc_id:
	MODMEMID(id, RRC_R, 23)

rrc_a:
	RRC_R(RA)
	TIME(8)

rl_b:
	RL_R(RB)
	TIME(8)

rl_c:
	RL_R(RC)
	TIME(8)

rl_d:
	RL_R(MD)
	TIME(8)

rl_e:
	RL_R(ME)
	TIME(8)

rl_h:
	RL_R(RH)
	TIME(8)

rl_l:
	RL_R(RL)
	TIME(8)

rl_ihl:
	MODMEM(RL_R, 15)

rl_id:
	MODMEMID(id, RL_R, 23)
	
rl_a:
	RL_R(RA)
	TIME(8)

rr_b:
	RR_R(RB)
	TIME(8)

rr_c:
	RR_R(RC)
	TIME(8)

rr_d:
	RR_R(MD)
	TIME(8)

rr_e:
	RR_R(ME)
	TIME(8)

rr_h:
	RR_R(RH)
	TIME(8)

rr_l:
	RR_R(RL)
	TIME(8)

rr_ihl:
	MODMEM(RR_R, 15)

rr_id:
	MODMEMID(id, RR_R, 23)

rr_a:
	RR_R(RA)
	TIME(8)


sla_b:
	SLA_R(RB)
	TIME(8)

sla_c:
	SLA_R(RC)
	TIME(8)

sla_d:
	SLA_R(MD)
	TIME(8)

sla_e:
	SLA_R(ME)
	TIME(8)

sla_h:
	SLA_R(RH)
	TIME(8)

sla_l:
	SLA_R(RL)
	TIME(8)

sla_ihl:
	MODMEM(SLA_R, 15)

sla_id:
	MODMEMID(id, SLA_R, 23)
	
sla_a:
	SLA_R(RA)
	TIME(8)


sra_b:
	SRA_R(RB)
	TIME(8)

sra_c:
	SRA_R(RC)
	TIME(8)

sra_d:
	SRA_R(MD)
	TIME(8)

sra_e:
	SRA_R(ME)
	TIME(8)

sra_h:
	SRA_R(RH)
	TIME(8)

sra_l:
	SRA_R(RL)
	TIME(8)

sra_ihl:
	MODMEM(SRA_R, 15)

sra_id:
	MODMEMID(id, SRA_R, 23)

sra_a:
	SRA_R(RA)
	TIME(8)


sll_b:
	SLL_R(RB)
	TIME(8)

sll_c:
	SLL_R(RC)
	TIME(8)

sll_d:
	SLL_R(MD)
	TIME(8)

sll_e:
	SLL_R(ME)
	TIME(8)

sll_h:
	SLL_R(RH)
	TIME(8)

sll_l:
	SLL_R(RL)
	TIME(8)

sll_ihl:
	MODMEM(SLL_R, 15)

sll_id:
	MODMEMID(id, SLL_R, 23)
	
sll_a:
	SLL_R(RA)
	TIME(8)


srl_b:
	SRL_R(RB)
	TIME(8)

srl_c:
	SRL_R(RC)
	TIME(8)

srl_d:
	SRL_R(MD)
	TIME(8)

srl_e:
	SRL_R(ME)
	TIME(8)

srl_h:
	SRL_R(RH)
	TIME(8)

srl_l:
	SRL_R(RL)
	TIME(8)

srl_ihl:
	MODMEM(SRL_R, 15)

srl_id:
	MODMEMID(id, SRL_R, 23)
	
srl_a:
	SRL_R(RA)
	TIME(8)



bit_0_b:
	BIT(0, RB)
	TIME(8)

bit_0_c:
	BIT(0, RC)
	TIME(8)

bit_0_d:
	BIT(0, MD)
	TIME(8)

bit_0_e:
	BIT(0, ME)
	TIME(8)

bit_0_h:
	BIT(0, RH)
	TIME(8)

bit_0_l:
	BIT(0, RL)
	TIME(8)

bit_0_ihl:
	BIT(0, (HLP))
	TIME(12)
	
bit_0_id:
	BIT(0, (HLP))
	RSHL
	TIMEX(20)

bit_0_a:
	BIT(0, RA)
	TIME(8)


bit_1_b:
	BIT(1, RB)
	TIME(8)

bit_1_c:
	BIT(1, RC)
	TIME(8)

bit_1_d:
	BIT(1, MD)
	TIME(8)

bit_1_e:
	BIT(1, ME)
	TIME(8)

bit_1_h:
	BIT(1, RH)
	TIME(8)

bit_1_l:
	BIT(1, RL)
	TIME(8)

bit_1_ihl:
	BIT(1, (HLP))
	TIME(12)
	
bit_1_id:
	BIT(1, (HLP))
	RSHL
	TIMEX(20)

bit_1_a:
	BIT(1, RA)
	TIME(8)



bit_2_b:
	BIT(2, RB)
	TIME(8)

bit_2_c:
	BIT(2, RC)
	TIME(8)

bit_2_d:
	BIT(2, MD)
	TIME(8)

bit_2_e:
	BIT(2, ME)
	TIME(8)

bit_2_h:
	BIT(2, RH)
	TIME(8)

bit_2_l:
	BIT(2, RL)
	TIME(8)

bit_2_ihl:
	BIT(2, (HLP))
	TIME(12)
	
bit_2_id:
	BIT(2, (HLP))
	RSHL
	TIMEX(20)

bit_2_a:
	BIT(2, RA)
	TIME(8)


bit_3_b:
	BIT(3, RB)
	TIME(8)

bit_3_c:
	BIT(3, RC)
	TIME(8)

bit_3_d:
	BIT(3, MD)
	TIME(8)

bit_3_e:
	BIT(3, ME)
	TIME(8)

bit_3_h:
	BIT(3, RH)
	TIME(8)

bit_3_l:
	BIT(3, RL)
	TIME(8)

bit_3_ihl:
	BIT(3, (HLP))
	TIME(12)
	
bit_3_id:
	BIT(3, (HLP))
	RSHL
	TIMEX(20)

bit_3_a:
	BIT(3, RA)
	TIME(8)


bit_4_b:
	BIT(4, RB)
	TIME(8)

bit_4_c:
	BIT(4, RC)
	TIME(8)

bit_4_d:
	BIT(4, MD)
	TIME(8)

bit_4_e:
	BIT(4, ME)
	TIME(8)

bit_4_h:
	BIT(4, RH)
	TIME(8)

bit_4_l:
	BIT(4, RL)
	TIME(8)

bit_4_ihl:
	BIT(4, (HLP))
	TIME(12)
	
bit_4_id:
	BIT(4, (HLP))
	RSHL
	TIMEX(20)

bit_4_a:
	BIT(4, RA)
	TIME(8)



bit_5_b:
	BIT(5, RB)
	TIME(8)

bit_5_c:
	BIT(5, RC)
	TIME(8)

bit_5_d:
	BIT(5, MD)
	TIME(8)

bit_5_e:
	BIT(5, ME)
	TIME(8)

bit_5_h:
	BIT(5, RH)
	TIME(8)

bit_5_l:
	BIT(5, RL)
	TIME(8)

bit_5_ihl:
	BIT(5, (HLP))
	TIME(12)
	
bit_5_id:
	BIT(5, (HLP))
	RSHL
	TIMEX(20)

bit_5_a:
	BIT(5, RA)
	TIME(8)



bit_6_b:
	BIT(6, RB)
	TIME(8)

bit_6_c:
	BIT(6, RC)
	TIME(8)

bit_6_d:
	BIT(6, MD)
	TIME(8)

bit_6_e:
	BIT(6, ME)
	TIME(8)

bit_6_h:
	BIT(6, RH)
	TIME(8)

bit_6_l:
	BIT(6, RL)
	TIME(8)

bit_6_ihl:
	BIT(6, (HLP))
	TIME(12)
	
bit_6_id:
	BIT(6, (HLP))
	RSHL
	TIMEX(20)

bit_6_a:
	BIT(6, RA)
	TIME(8)



bit_7_b:
	BIT(7, RB)
	TIME(8)

bit_7_c:
	BIT(7, RC)
	TIME(8)

bit_7_d:
	BIT(7, MD)
	TIME(8)

bit_7_e:
	BIT(7, ME)
	TIME(8)

bit_7_h:
	BIT(7, RH)
	TIME(8)

bit_7_l:
	BIT(7, RL)
	TIME(8)

bit_7_ihl:
	BIT(7, (HLP))
	TIME(12)
	
bit_7_id:
	BIT(7, (HLP))
	RSHL
	TIMEX(20)

bit_7_a:
	BIT(7, RA)
	TIME(8)


#define PUTMEM_HLP_RW_15 \
	jmp pm15

#define PUTMEMID_HLP_RW_23 \
	jmp pm23

pm15:
	PUTMEM_RHL(pm15, RW, 15)
	
pm23:
	PUTMEMID_RHL(pm23, RW, 23)


res_0_b:
	RES(0, RB)
	TIME(8)

res_0_c:
	RES(0, RC)
	TIME(8)

res_0_d:
	RES(0, MD)
	TIME(8)

res_0_e:
	RES(0, ME)
	TIME(8)

res_0_h:
	RES(0, RH)
	TIME(8)

res_0_l:
	RES(0, RL)
	TIME(8)

res_0_ihl:
	movb (HLP), RW
	RES(0, RW)
	PUTMEM_HLP_RW_15
	
res_0_id:
	movb (HLP), RW
	RES(0, RW)
	PUTMEMID_HLP_RW_23

res_0_a:
	RES(0, RA)
	TIME(8)



res_1_b:
	RES(1, RB)
	TIME(8)

res_1_c:
	RES(1, RC)
	TIME(8)

res_1_d:
	RES(1, MD)
	TIME(8)

res_1_e:
	RES(1, ME)
	TIME(8)

res_1_h:
	RES(1, RH)
	TIME(8)

res_1_l:
	RES(1, RL)
	TIME(8)

res_1_ihl:
	movb (HLP), RW
	RES(1, RW)
	PUTMEM_HLP_RW_15
	
res_1_id:
	movb (HLP), RW
	RES(1, RW)
	PUTMEMID_HLP_RW_23

res_1_a:
	RES(1, RA)
	TIME(8)



res_2_b:
	RES(2, RB)
	TIME(8)

res_2_c:
	RES(2, RC)
	TIME(8)

res_2_d:
	RES(2, MD)
	TIME(8)

res_2_e:
	RES(2, ME)
	TIME(8)

res_2_h:
	RES(2, RH)
	TIME(8)

res_2_l:
	RES(2, RL)
	TIME(8)

res_2_ihl:
	movb (HLP), RW
	RES(2, RW)
	PUTMEM_HLP_RW_15
	
res_2_id:
	movb (HLP), RW
	RES(2, RW)
	PUTMEMID_HLP_RW_23

res_2_a:
	RES(2, RA)
	TIME(8)



res_3_b:
	RES(3, RB)
	TIME(8)

res_3_c:
	RES(3, RC)
	TIME(8)

res_3_d:
	RES(3, MD)
	TIME(8)

res_3_e:
	RES(3, ME)
	TIME(8)

res_3_h:
	RES(3, RH)
	TIME(8)

res_3_l:
	RES(3, RL)
	TIME(8)

res_3_ihl:
	movb (HLP), RW
	RES(3, RW)
	PUTMEM_HLP_RW_15
	
res_3_id:
	movb (HLP), RW
	RES(3, RW)
	PUTMEMID_HLP_RW_23

res_3_a:
	RES(3, RA)
	TIME(8)



res_4_b:
	RES(4, RB)
	TIME(8)

res_4_c:
	RES(4, RC)
	TIME(8)

res_4_d:
	RES(4, MD)
	TIME(8)

res_4_e:
	RES(4, ME)
	TIME(8)

res_4_h:
	RES(4, RH)
	TIME(8)

res_4_l:
	RES(4, RL)
	TIME(8)

res_4_ihl:
	movb (HLP), RW
	RES(4, RW)
	PUTMEM_HLP_RW_15
	
res_4_id:
	movb (HLP), RW
	RES(4, RW)
	PUTMEMID_HLP_RW_23

res_4_a:
	RES(4, RA)
	TIME(8)



res_5_b:
	RES(5, RB)
	TIME(8)

res_5_c:
	RES(5, RC)
	TIME(8)

res_5_d:
	RES(5, MD)
	TIME(8)

res_5_e:
	RES(5, ME)
	TIME(8)

res_5_h:
	RES(5, RH)
	TIME(8)

res_5_l:
	RES(5, RL)
	TIME(8)

res_5_ihl:
	movb (HLP), RW
	RES(5, RW)
	PUTMEM_HLP_RW_15
	
res_5_id:
	movb (HLP), RW
	RES(5, RW)
	PUTMEMID_HLP_RW_23

res_5_a:
	RES(5, RA)
	TIME(8)



res_6_b:
	RES(6, RB)
	TIME(8)

res_6_c:
	RES(6, RC)
	TIME(8)

res_6_d:
	RES(6, MD)
	TIME(8)

res_6_e:
	RES(6, ME)
	TIME(8)

res_6_h:
	RES(6, RH)
	TIME(8)

res_6_l:
	RES(6, RL)
	TIME(8)

res_6_ihl:
	movb (HLP), RW
	RES(6, RW)
	PUTMEM_HLP_RW_15
	
res_6_id:
	movb (HLP), RW
	RES(6, RW)
	PUTMEMID_HLP_RW_23

res_6_a:
	RES(6, RA)
	TIME(8)



res_7_b:
	RES(7, RB)
	TIME(8)

res_7_c:
	RES(7, RC)
	TIME(8)

res_7_d:
	RES(7, MD)
	TIME(8)

res_7_e:
	RES(7, ME)
	TIME(8)

res_7_h:
	RES(7, RH)
	TIME(8)

res_7_l:
	RES(7, RL)
	TIME(8)

res_7_ihl:
	movb (HLP), RW
	RES(7, RW)
	PUTMEM_HLP_RW_15
	
res_7_id:
	movb (HLP), RW
	RES(7, RW)
	PUTMEMID_HLP_RW_23

res_7_a:
	RES(7, RA)
	TIME(8)



set_0_b:
	SET(0, RB)
	TIME(8)

set_0_c:
	SET(0, RC)
	TIME(8)

set_0_d:
	SET(0, MD)
	TIME(8)

set_0_e:
	SET(0, ME)
	TIME(8)

set_0_h:
	SET(0, RH)
	TIME(8)

set_0_l:
	SET(0, RL)
	TIME(8)

set_0_ihl:
	movb (HLP), RW
	SET(0, RW)
	PUTMEM_HLP_RW_15
	
set_0_id:
	movb (HLP), RW
	SET(0, RW)
	PUTMEMID_HLP_RW_23

set_0_a:
	SET(0, RA)
	TIME(8)



set_1_b:
	SET(1, RB)
	TIME(8)

set_1_c:
	SET(1, RC)
	TIME(8)

set_1_d:
	SET(1, MD)
	TIME(8)

set_1_e:
	SET(1, ME)
	TIME(8)

set_1_h:
	SET(1, RH)
	TIME(8)

set_1_l:
	SET(1, RL)
	TIME(8)

set_1_ihl:
	movb (HLP), RW
	SET(1, RW)
	PUTMEM_HLP_RW_15
	
set_1_id:
	movb (HLP), RW
	SET(1, RW)
	PUTMEMID_HLP_RW_23

set_1_a:
	SET(1, RA)
	TIME(8)



set_2_b:
	SET(2, RB)
	TIME(8)

set_2_c:
	SET(2, RC)
	TIME(8)

set_2_d:
	SET(2, MD)
	TIME(8)

set_2_e:
	SET(2, ME)
	TIME(8)

set_2_h:
	SET(2, RH)
	TIME(8)

set_2_l:
	SET(2, RL)
	TIME(8)

set_2_ihl:
	movb (HLP), RW
	SET(2, RW)
	PUTMEM_HLP_RW_15
	
set_2_id:
	movb (HLP), RW
	SET(2, RW)
	PUTMEMID_HLP_RW_23

set_2_a:
	SET(2, RA)
	TIME(8)



set_3_b:
	SET(3, RB)
	TIME(8)

set_3_c:
	SET(3, RC)
	TIME(8)

set_3_d:
	SET(3, MD)
	TIME(8)

set_3_e:
	SET(3, ME)
	TIME(8)

set_3_h:
	SET(3, RH)
	TIME(8)

set_3_l:
	SET(3, RL)
	TIME(8)

set_3_ihl:
	movb (HLP), RW
	SET(3, RW)
	PUTMEM_HLP_RW_15
	
set_3_id:
	movb (HLP), RW
	SET(3, RW)
	PUTMEMID_HLP_RW_23

set_3_a:
	SET(3, RA)
	TIME(8)



set_4_b:
	SET(4, RB)
	TIME(8)

set_4_c:
	SET(4, RC)
	TIME(8)

set_4_d:
	SET(4, MD)
	TIME(8)

set_4_e:
	SET(4, ME)
	TIME(8)

set_4_h:
	SET(4, RH)
	TIME(8)

set_4_l:
	SET(4, RL)
	TIME(8)

set_4_ihl:
	movb (HLP), RW
	SET(4, RW)
	PUTMEM_HLP_RW_15
	
set_4_id:
	movb (HLP), RW
	SET(4, RW)
	PUTMEMID_HLP_RW_23

set_4_a:
	SET(4, RA)
	TIME(8)



set_5_b:
	SET(5, RB)
	TIME(8)

set_5_c:
	SET(5, RC)
	TIME(8)

set_5_d:
	SET(5, MD)
	TIME(8)

set_5_e:
	SET(5, ME)
	TIME(8)

set_5_h:
	SET(5, RH)
	TIME(8)

set_5_l:
	SET(5, RL)
	TIME(8)

set_5_ihl:
	movb (HLP), RW
	SET(5, RW)
	PUTMEM_HLP_RW_15
	
set_5_id:
	movb (HLP), RW
	SET(5, RW)
	PUTMEMID_HLP_RW_23

set_5_a:
	SET(5, RA)
	TIME(8)



set_6_b:
	SET(6, RB)
	TIME(8)

set_6_c:
	SET(6, RC)
	TIME(8)

set_6_d:
	SET(6, MD)
	TIME(8)

set_6_e:
	SET(6, ME)
	TIME(8)

set_6_h:
	SET(6, RH)
	TIME(8)

set_6_l:
	SET(6, RL)
	TIME(8)

set_6_ihl:
	movb (HLP), RW
	SET(6, RW)
	PUTMEM_HLP_RW_15
	
set_6_id:
	movb (HLP), RW
	SET(6, RW)
	PUTMEMID_HLP_RW_23

set_6_a:
	SET(6, RA)
	TIME(8)



set_7_b:
	SET(7, RB)
	TIME(8)

set_7_c:
	SET(7, RC)
	TIME(8)

set_7_d:
	SET(7, MD)
	TIME(8)

set_7_e:
	SET(7, ME)
	TIME(8)

set_7_h:
	SET(7, RH)
	TIME(8)

set_7_l:
	SET(7, RL)
	TIME(8)

set_7_ihl:
	movb (HLP), RW
	SET(7, RW)
	PUTMEM_HLP_RW_15
	
set_7_id:
	movb (HLP), RW
	SET(7, RW)
	PUTMEMID_HLP_RW_23

set_7_a:
	SET(7, RA)
	TIME(8)
