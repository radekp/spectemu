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

/* instructions prefixed by ED */

#define IN_R_IC(rd) \
	IN(in_ ## rd ## _ic, RB, RC, rd); \
	testb rd, rd		; \
	CFLAG(BUTCF, SZPF)	; \
	TIME(12)
	
#define IN_M_IC(rd) \
	xchgb rd, RA		; \
	IN(in_ ## rd ## _ic, RB, RC, RA); \
	testb RA, RA		; \
	CFLAG(BUTCF, SZPF)	; \
	xchgb rd, RA		; \
	TIME(12)

in_b_ic:
	IN_R_IC(RB)

in_c_ic:
	IN_R_IC(RC)

in_d_ic:
	IN_M_IC(MD)

in_e_ic:
	IN_M_IC(ME)

in_h_ic:
	IN_R_IC(RH)

in_l_ic:
	IN_R_IC(RL)

in_f_ic:
	IN_R_IC(RW) /* No load in registers */

in_a_ic:
	IN_R_IC(RA)


	
#define OUT_IC_R(rs) \
	OUT(out_ic_ ## rs, RB, RC, rs); \
	TIME(12)

#define OUT_IC_M(rs) \
	xchgb rs, RA		; \
	OUT(out_ic_ ## rs, RB, RC, RA); \
	xchgb rs, RA		; \
	TIME(12)

	
out_ic_b:
	OUT_IC_R(RB)

out_ic_c:
	OUT_IC_R(RC)

out_ic_d:
	OUT_IC_M(MD)

out_ic_e:
	OUT_IC_M(ME)

out_ic_h:
	OUT_IC_R(RH)

out_ic_l:
	OUT_IC_R(RL)

out_ic_0:
	xorb RW,RW
	OUT_IC_R(RW)

out_ic_a:
	OUT_IC_R(RA)


#define SBC_HL_RR(rh, rl) \
	sbbb rl, RL		; \
	sbbb rh, RH		; \
	pushfl			; \
	testw RHL, RHL		; \
	SFLAG(ALLF, ZF)		; \
	popfl			; \
	VSFLAG(SHCF, SHCF)	; \
	TIME(15)
	
sbc_hl_bc:
	SFLAGS
	SBC_HL_RR(RB, RC)

sbc_hl_de:
	SFLAGS
	SBC_HL_RR(MD, ME)

sbc_hl_hl:
	SFLAGS
	SBC_HL_RR(RH, RL)

sbc_hl_sp:
	SFLAGS
	movw RSP, RVW
	SBC_HL_RR(RV, RW)



#define ADC_HL_RR(rh, rl) \
	adcb rl, RL		; \
	adcb rh, RH		; \
	pushfl			; \
	testw RHL, RHL		; \
	CFLAG(ALLF, ZF)		; \
	popfl			; \
	VCFLAG(SHCF, SHCF)	; \
	TIME(15)

adc_hl_bc:
	SFLAGS
	ADC_HL_RR(RB, RC)

adc_hl_de:
	SFLAGS
	ADC_HL_RR(MD, ME)

adc_hl_hl:
	SFLAGS
	ADC_HL_RR(RH, RL)

adc_hl_sp:
	movw RSP, RVW
	SFLAGS
	ADC_HL_RR(RV, RW)


#define LD_INN_RR(rn, rh, rl) \
	DFETCHP			; \
	PUTMEM_RVW_NE(ld_inn1_ ## rn, rl); \
	incw RVW		; \
	PUTMEM_RVW_NE(ld_inn2_ ## rn, rh)

ld_inn_bc:
	LD_INN_RR(bc, RB, RC)
	TIME(20)

ld_inn_de:
	xchgw MDE, RHL
	LD_INN_RR(de, RH, RL)
	xchgw MDE, RHL
	TIME(20)

ld_inn_hl_ed:
	LD_INN_RR(hl, RH, RL)
	TIME(20)

ld_inn_sp:
	xchgw RSP, RHL
	LD_INN_RR(sp, RH, RL)
	xchgw RSP, RHL
	TIME(20)

#define LD_RR_INN(rh, rl) \
	DFETCHP			; \
	movb (VWP), rl		; \
	incw RVW		; \
	movb (VWP), rh
		

ld_bc_inn:
	LD_RR_INN(RB, RC)
	TIME(20)
	
ld_de_inn:
	xchgw MDE, RHL
	LD_RR_INN(RH, RL)
	xchgw MDE, RHL
	TIME(20)
	
ld_hl_inn_ed:
	LD_RR_INN(RH, RL)
	TIME(20)
	
ld_sp_inn:
	xchgw RSP, RHL
	LD_RR_INN(RH, RL)
	xchgw RSP, RHL
	TIME(20)
	
neg:
	negb RA
	SUBFLAGS
	TIME(8)


retn:
	movl IFF2, VWP
	movl VWP, IFF1
	POPD(RPC)
	TIME(14)

reti:
	POPD(RPC)
	TIME(14)

im_0:
	movl $0, IT_MODE
	TIME(8)
	
im_1:
	movl $1, IT_MODE
	TIME(8)
	
im_2:
	movl $2, IT_MODE
	TIME(8)
	

ld_i_a:
	movb RA, MI
	TIME(9)

ld_r_a:
	movb RA, RL7
	movb RA, RW
	andb $0x80, RW
	movb RW, MR
	TIME(9)


#define AIRFLAGS \
	testb RA, RA		; \
	CFLAG(BUTCF, SZF)	; \
	movl IFF2, VWP	        ; \
	testl VWP, VWP		; \
	jz ld_air_end		; \
	orb $PVF, RF		; \
	TIME(9)			; \
	ALIGN

ld_air_end:
	andb $~PVF, RF
	TIME(9)
	
ld_a_i:		
	movb MI, RA
	AIRFLAGS
	
ld_a_r:
	movb MR, RA
	movb RL7, RW
	andb $0x7F, RW
	orb RW, RA
	AIRFLAGS
	
rrd:
	movb RA, RV
	movb (HLP), RW
	shlw $4, RVW
	shrb $4, RW
	andb $0xF0, RA
	orb RW, RA
	movb RV, RW
	CFLAG(BUTCF, SZPF)
	PUTMEM_RHL(rrd, RW, 18)

rld:	
	movb RA, RW
	movb (HLP), RV
	shlb $4, RW
	shrw $4, RVW
	andb $0xF0, RA
	orb RV, RA
	CFLAG(BUTCF, SZPF)
	PUTMEM_RHL(rld, RW, 18)

#define REPEAT \
	subw $2, RPC
	
#define LDID(l, dir) \
	movb (HLP), RW		; \
	xchgw RHL, MDE		; \
	PUTMEM_RHL_NE(l, RW)	; \
	dir RHL			; \
	xchgw RHL, MDE		; \
	dir RHL			; \
	andb $~HPNF, RF		; \
	decw RBC		; \
	jz ldid			; \
	orb $PVF, RF


ldid:
	TIME(16)
	
		
ldi:
	LDID(ldi, incw)
	TIME(16)

ldir:
	LDID(ldir, incw)
	REPEAT
	TIME(21)

ldd:
	LDID(ldd, decw)
	TIME(16)
	
lddr:
	LDID(lddr, decw)
	REPEAT
	TIME(21)

#define CPID(dir) \
	cmpb (HLP), RA		; \
	SFLAG(BUTCF, SZHF)	; \
	dir RHL			; \
	decw RBC		; \
	jz cpid			; \
	JCOND(ZF, ldid)

cpid:
	andb $~PVF, RF
	TIME(16)
	
cpi:
	CPID(incw)
	TIME(16)

cpir:
	CPID(incw)
	REPEAT
	TIME(21)
	
cpd:
	CPID(decw)
	TIME(16)

cpdr:
	CPID(decw)
	REPEAT
	TIME(21)
	
#define INID(l, dir) \
	decb RB			; \
	VSFLAG(BUTCF, SZHF)	; \
	IN(l, RB, RC, RW)	; \
	PUTMEM_RHL_NE(l, RW)	; \
	dir RHL			; \
	testb RB, RB		; \
	jz ldid
	
ini:
	INID(ini, incw)
	TIME(16)

inir:
	INID(inir, incw)
	REPEAT
	TIME(21)
	
ind:
	INID(ind, decw)
	TIME(16)

indr:
	INID(indr, decw)
	REPEAT
	TIME(21)
	
#define OTID(l, dir) \
	decb RB			; \
	VSFLAG(BUTCF, SZHF)	; \
	movb (HLP), RW		; \
	OUT(l, RB, RC, RW)	; \
	dir RHL			; \
	testb RB, RB		; \
	jz ldid
	
outi:
	OTID(outi, incw)
	TIME(16)

otir:
	OTID(otir, incw)
	REPEAT
	TIME(21)
	
outd:
	OTID(outd, decw)
	TIME(16)

otdr:
	INID(otdr, decw)
	REPEAT
	TIME(21)

ill_ed:
	TIME(8)
