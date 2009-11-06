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

.text
	
#include "i386def.S"

#ifndef COMPARISON	
#define Z80(s) NAME(z80_ ## s)
#else
#define Z80(s) NAME(z80x_ ## s)
#endif

#define NR 0
#define BR 36
#define OT 52

#define DAT(t, o) Z80(proc)+t+o

#define NUMDREGS 9

#define BBC DAT(BR,0)
#define BDE DAT(BR,4)
#define BHL DAT(BR,8)
#define BAF DAT(BR,12)
	
#define MBC DAT(NR,0)
#define MDE DAT(NR,4)
#define MHL DAT(NR,8)
#define MAF DAT(NR,12)
#define MIX DAT(NR,20)
#define MIY DAT(NR,24)
#define MPC DAT(NR,28)
#define MSP DAT(NR,32)

#define MC  DAT(NR,0)
#define MB  DAT(NR,1)
#define ME  DAT(NR,4)
#define MD  DAT(NR,5)
#define ML  DAT(NR,8)
#define MH  DAT(NR,9)
#define MF  DAT(NR,12)
#define MA  DAT(NR,13)
#define MR  DAT(NR,16)
#define MI  DAT(NR,17)

#define MIXL DAT(NR,20)
#define MIXH DAT(NR,21)
#define MIYL DAT(NR,24)
#define MIYH DAT(NR,25)

#define HALTSTATE DAT(OT,0)
#define IT_MODE   DAT(OT,4)
#define IFF1      DAT(OT,8)
#define IFF2      DAT(OT,12)
/* #define MEM       DAT(OT,16) */
/* #define REAL_TC   DAT(OT,20) */
#define RL7       DAT(OT,24)
		
#define RV %ah
#define RW %al
#define RVW %ax
#define VWP %eax
	
#define RA  %ch
#define RF  %cl
#define RAF %cx
#define SCRP %ecx

#define RB  %bh
#define RC  %bl
#define RBC %bx
#define BCP %ebx

#define RH  %dh
#define RL  %dl
#define RHL %dx
#define HLP %edx

#define RPC %si
#define PCP %esi

#define RSP %bp
#define SPP %ebp

#define INC_R incl RL7

#define DEC_R decl RL7

	
#define EOI(x) \
	INC_R			; \
	subl $ x,TC		; \
	jng loop_end

#define IBR \
	jmp *intr_table(,VWP,4)
	
#define TIME(x) \
	EOI(x)			; \
	FETCHL			; \
	IBR			; \
	ALIGN 

#define TIMEX(x) \
	EOI((x-4))              ; \
	FETCHL			; \
	IBR			; \
	ALIGN 
	
	
#define TIME_NC(x) \
	EOI(x)			; \
	FETCH			; \
	IBR			; \
	ALIGN
			
#define SF   128
#define ZF   64
#define HF   16
#define PVF  4
#define NF   2
#define CF   1

#define ALLF 0xD7
#define BUTCF 0xD6
#define SZHF 0xD0
#define SZF 0xC0
#define SZHCF 0xD1
#define SHCF 0x91
#define HNCF 0x13
#define HNF 0x12
#define ZNF 0x42
#define HPNF 0x16
#define HCF 0x11
#define BUTNF 0xD5
#define SZPF 0xC4
#define SZPCF 0xC5
#define ZPF  0x44
#define SZPNF 0xC6
#define SZNF 0xC2

#define VCFLAG(clear, change) \
	lahf			; \
	seto RW			; \
	andb $~ clear, RF	; \
	andb $ change, RV	; \
	orb RV, RF		; \
	shlb $2, RW		; \
	orb RW, RF

#define VSFLAG(set, change) \
	lahf			; \
	setno RW		; \
	orb $ set, RF	; \
	orb $~ change, RV	; \
	andb RV, RF		; \
	shlb $2, RW		; \
	notb RW			; \
	andb RW, RF	

#define ALLFLAG() \
	lahf			; \
	movb RV, RF
	
#define CFLAG(clear, change) \
	lahf			; \
	andb $~ clear, RF	; \
	andb $ change, RV	; \
	orb RV, RF

#define SFLAG(set, change) \
	lahf			; \
	orb $ set, RF	; \
	orb $~ change, RV	; \
	andb RV, RF

#define CSFLAG(clear, set, change) \
	lahf			; \
	andb $~ clear, RF	; \
	orb $ set, RF	; \
	andb $ change, RV	; \
	orb RV, RF
		
#define COND(fl) testb $ fl, RF

#define JCOND(fl, l) \
	COND(fl)		; \
	jnz l

#define JNCOND(fl, l) \
	COND(fl)		; \
	jz l
		

#define INCPC incw RPC
	
#define DINCPC \
	INCPC			; \
	INCPC
	
#define SFLAGS \
	movb RF, RV		; \
	sahf
			
#define FETCHR(ir) \
	movb (PCP), ir		; \
	INCPC

#define FETCHS \
	movsbw (PCP), RVW	; \
	INCPC

#define FETCHL \
	movzbl (PCP), VWP	; \
	INCPC
	
#define FETCH FETCHR(RW)
		
#define DFETCH \
	FETCHR(RW)		; \
	FETCHR(RV)

#define DFETCHP \
	movl HLP, VWP	; \
	DFETCH


	
#ifdef SPECT_MEM

#include "i386sp.S"
	
#else /* SPECT_MEM */

#define PUTMEM_REG(l, reg, regh, regp, val, endf) \
	movb val, (regp)	; \
	endf
	
#define PUTMEM_REG_NE(l, reg, regh, regp, val) \
	movb val, (regp)
	
#define MODMEMF(l, func, endf) \
	func((HLP))		; \
	endf

#define PUSH(l, rh, rl) \
	decw RSP		; \
	movb rh, (SPP)		; \
	decw RSP		; \
	movb rl, (SPP)

#define PUSHT(l, rh, rl, t) \
	PUSH(l, rh, rl)		; \
	TIME(t)

#define IN(l, porth, portl, rd) \
	movzbl portl, VWP	; \
	movb Z80(inports)(VWP), rd

#define OUT(l, porth, portl, rs) \
	movzbl portl, VWP	; \
	movb rs, Z80(outports)(VWP)

#define DI_CHECK		
	
#endif

#define RSHLTIME(time) \
	RSHL			; \
	TIMEX(time)
	
#define MODMEM(func, time) \
	MODMEMF(func, func, TIME(time))
	
#define MODMEMID(l, func, time) \
	MODMEMF(func ## l, func, RSHLTIME(time))
	

#define PUTMEM_RVW(l, val, time) \
	PUTMEM_REG(l, RVW, RV, VWP, val, TIME(time))

#define PUTMEM_RHL(l, val, time) \
	PUTMEM_REG(l, RHL, RH, HLP, val, TIME(time))

#define PUTMEM_RBC(l, val, time) \
	PUTMEM_REG(l, RBC, RB, BCP, val, TIME(time))

#define PUTMEMID_RHL(l, val, time) \
	PUTMEM_REG(l, RHL, RH, HLP, val, RSHLTIME(time)) 
		
#define PUTMEM_RVW_NE(l, val) \
	PUTMEM_REG_NE(l, RVW, RV, VWP, val)

#define PUTMEM_RHL_NE(l, val) \
	PUTMEM_REG_NE(l, RHL, RH, HLP, val)


#define PUSHD(l, dr) \
	movw dr, RVW		; \
	PUSH(l, RV, RW)
	
#define POP(rh, rl) \
	movb (SPP), rl		; \
	incw RSP		; \
	movb (SPP), rh		; \
	incw RSP

#define POPD(dr) \
	POP(RV, RW)		; \
	movw RVW, dr

	
#define ADDFL VCFLAG(ALLF, SZHCF)
#define SUBFL VSFLAG(ALLF, SZHCF)
#define ORFL CFLAG(ALLF, SZPF)
#define ANDFL CSFLAG(ALLF, HF, SZPF)

#ifdef FASTFLAGS
	
#define ADDFLAGS ADDFL
#define SUBFLAGS SUBFL
#define ORFLAGS ORFL
#define ANDFLAGS ANDFL

#else
	ALIGN
	
addflags:
	ADDFL		; ret; ALIGN
subflags:	
	SUBFL		; ret; ALIGN
orflags:	
	ORFL		; ret; ALIGN
andflags:	
	ANDFL		; ret; ALIGN

#define ADDFLAGS call addflags
#define SUBFLAGS call subflags
#define ORFLAGS call orflags
#define ANDFLAGS call andflags
	
#endif /* fastflags */
	
#define IXDGET \
	pushl HLP		; \
	movw MIX, RHL		; \
	FETCHS			; \
	addw RVW, RHL		; \

#define IYDGET \
	pushl HLP		; \
	movw MIY, RHL		; \
	FETCHS			; \
	addw RVW, RHL		; \

#define RSHL \
	popl HLP
		
#define ADD_A_R(r) \
	addb r, RA		; \
	ADDFLAGS
	
#define ADC_A_R(r) \
	SFLAGS			; \
	adcb r, RA		; \
	ADDFLAGS

#define SUB_R(r) \
	subb r, RA		; \
	SUBFLAGS
	
#define SBC_A_R(r) \
	SFLAGS			; \
	sbbb r, RA		; \
	SUBFLAGS

#define AND_R(r) \
	andb r, RA		; \
	ANDFLAGS

#define XOR_R(r) \
	xorb r, RA		; \
	ORFLAGS
	
#define OR_R(r) \
	orb r, RA		; \
	ORFLAGS
	
#define CP_R(r)	\
	cmpb r, RA		; \
	SUBFLAGS
		

	ALIGN

.globl Z80(local_init)
/* .type    Z80(local_init),@function */
Z80(local_init):
/* Nothing to initialize */
	ret
				
	ALIGN

.globl Z80(pushpc)
/* .type    Z80(pushpc),@function */
Z80(pushpc):
	pushl SPP
	pushl PCP

	movl MPC, PCP
	movl MSP, SPP
	xorl SCRP, SCRP

	PUSHD(z80_pushpci, RPC)
	
	movl SPP, MSP
	
	popl PCP
	popl SPP

	ret

	ALIGN

.globl Z80(step)
/* .type    Z80(step),@function */
Z80(step):
	movl 4(%esp), %eax
	
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx

	movl %eax, TC
	
	call load_regs
	call step_loop
	call store_regs

	movl TC, %eax	

	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret
	
	ALIGN
	
load_regs:
	movzwl MAF, SCRP
	movl MBC, BCP
	movl MHL, HLP
	movl MPC, PCP
	movl MSP, SPP

	movb MR, RW
	andb $0x80, MR
	movb RW, RL7
	ret
	ALIGN

store_regs:
	movw RAF, MAF
	movl BCP, MBC
	movl HLP, MHL
	movl PCP, MPC
	movl SPP, MSP

	movb RL7, RW
	andb $0x7F, RW
	orb RW, MR
	ret
	ALIGN
	
step_loop:
	cmpl $0, HALTSTATE
	jnz haltstate
	FETCHL
	IBR
	ALIGN
	
loop_end:
	ret
	ALIGN
	
special_cb:
	INC_R
	FETCHL
	jmp *intr_table_cb(,VWP,4)
	ALIGN

special_dd_cb:
	IXDGET
	FETCHL
	jmp *intr_table_dd_fd_cb(,VWP,4)
	ALIGN

special_fd_cb:
	IYDGET
	FETCHL
	jmp *intr_table_dd_fd_cb(,VWP,4)
	ALIGN	
	
special_dd:
	INC_R
	subl $4,TC
	FETCHL
	jmp *intr_table_dd(,VWP,4)
	ALIGN

	
special_ed:
	INC_R
	FETCHL
	jmp *intr_table_ed(,VWP,4)
	ALIGN

special_fd:
	INC_R
	subl $4,TC
	FETCHL
	jmp *intr_table_fd(,VWP,4)
	ALIGN

special_xx:
	DEC_R
	cmpl $0, TC
	jng loop_end
	IBR
	ALIGN
	

#include "i386op1.S"
#include "i386op2.S"
#include "i386op3.S"
#include "i386op4.S"
#include "i386op5.S"
#include "i386op6.S"

	.data
			
intr_table:
	.long nop		# 00
	.long ld_bc_nn
	.long ld_ibc_a
	.long inc_bc
	.long inc_b
	.long dec_b
	.long ld_b_n
	.long rlca
	.long ex_af_afb
	.long add_hl_bc
	.long ld_a_ibc
	.long dec_bc
	.long inc_c
	.long dec_c
	.long ld_c_n
	.long rrca

	.long djnz_e		# 10
	.long ld_de_nn
	.long ld_ide_a
	.long inc_de
	.long inc_d
	.long dec_d
	.long ld_d_n
	.long rla
	.long jr_e
	.long add_hl_de
	.long ld_a_ide
	.long dec_de
	.long inc_e
	.long dec_e
	.long ld_e_n
	.long rra

	.long jr_nz_e		# 20
	.long ld_hl_nn
	.long ld_inn_hl
	.long inc_hl
	.long inc_h
	.long dec_h
	.long ld_h_n
	.long daa
	.long jr_z_e
	.long add_hl_hl
	.long ld_hl_inn
	.long dec_hl
	.long inc_l
	.long dec_l
	.long ld_l_n
	.long cpl

	.long jr_nc_e		# 30
	.long ld_sp_nn
	.long ld_inn_a
	.long inc_sp
	.long inc_ihl
	.long dec_ihl
	.long ld_ihl_n
	.long scf
	.long jr_c_e
	.long add_hl_sp
	.long ld_a_inn
	.long dec_sp
	.long inc_a
	.long dec_a
	.long ld_a_n
	.long ccf

	.long nop		# 40
	.long ld_b_c
	.long ld_b_d
	.long ld_b_e
	.long ld_b_h
	.long ld_b_l
	.long ld_b_ihl
	.long ld_b_a
	.long ld_c_b
	.long nop
	.long ld_c_d
	.long ld_c_e
	.long ld_c_h
	.long ld_c_l
	.long ld_c_ihl
	.long ld_c_a

	.long ld_d_b		# 50
	.long ld_d_c
	.long nop
	.long ld_d_e
	.long ld_d_h
	.long ld_d_l
	.long ld_d_ihl
	.long ld_d_a
	.long ld_e_b
	.long ld_e_c
	.long ld_e_d
	.long nop
	.long ld_e_h
	.long ld_e_l
	.long ld_e_ihl
	.long ld_e_a

	.long ld_h_b		# 60
	.long ld_h_c
	.long ld_h_d
	.long ld_h_e
	.long nop
	.long ld_h_l
	.long ld_h_ihl
	.long ld_h_a
	.long ld_l_b
	.long ld_l_c
	.long ld_l_d
	.long ld_l_e
	.long ld_l_h
	.long nop
	.long ld_l_ihl
	.long ld_l_a
	
	.long ld_ihl_b		# 70
	.long ld_ihl_c
	.long ld_ihl_d
	.long ld_ihl_e
	.long ld_ihl_h
	.long ld_ihl_l
	.long halt
	.long ld_ihl_a
	.long ld_a_b
	.long ld_a_c
	.long ld_a_d
	.long ld_a_e
	.long ld_a_h
	.long ld_a_l
	.long ld_a_ihl
	.long nop
	
	.long add_a_b		# 80
	.long add_a_c
	.long add_a_d
	.long add_a_e
	.long add_a_h
	.long add_a_l
	.long add_a_ihl
	.long add_a_a
	.long adc_a_b
	.long adc_a_c
	.long adc_a_d
	.long adc_a_e
	.long adc_a_h
	.long adc_a_l
	.long adc_a_ihl
	.long adc_a_a
	
	.long sub_b		# 90
	.long sub_c
	.long sub_d
	.long sub_e
	.long sub_h
	.long sub_l
	.long sub_ihl
	.long sub_a
	.long sbc_a_b
	.long sbc_a_c
	.long sbc_a_d
	.long sbc_a_e
	.long sbc_a_h
	.long sbc_a_l
	.long sbc_a_ihl
	.long sbc_a_a

	.long and_b		# A0
	.long and_c
	.long and_d
	.long and_e
	.long and_h
	.long and_l
	.long and_ihl
	.long and_a
	.long xor_b
	.long xor_c
	.long xor_d
	.long xor_e
	.long xor_h
	.long xor_l
	.long xor_ihl
	.long xor_a

	.long or_b		# B0
	.long or_c
	.long or_d
	.long or_e
	.long or_h
	.long or_l
	.long or_ihl
	.long or_a
	.long cp_b
	.long cp_c
	.long cp_d
	.long cp_e
	.long cp_h
	.long cp_l
	.long cp_ihl
	.long cp_a

	.long ret_nz		# C0
	.long pop_bc
	.long jp_nz_nn
	.long jp_nn
	.long call_nz_nn
	.long push_bc
	.long add_a_n
	.long rst_00
	.long ret_z
	.long ret
	.long jp_z_nn
	.long special_cb
	.long call_z_nn
	.long call_nn
	.long adc_a_n
	.long rst_08

	.long ret_nc		# D0
	.long pop_de
	.long jp_nc_nn
	.long out_in_a
	.long call_nc_nn
	.long push_de
	.long sub_n
	.long rst_10
	.long ret_c
	.long exx
	.long jp_c_nn
	.long in_a_in
	.long call_c_nn
	.long special_dd
	.long sbc_a_n
	.long rst_18

	.long ret_po		# E0
	.long pop_hl
	.long jp_po_nn
	.long ex_isp_hl
	.long call_po_nn
	.long push_hl
	.long and_n
	.long rst_20
	.long ret_pe
	.long jp_hl
	.long jp_pe_nn
	.long ex_de_hl
	.long call_pe_nn
	.long special_ed
	.long xor_n
	.long rst_28

	.long ret_p		# F0
	.long pop_af
	.long jp_p_nn
	.long di
	.long call_p_nn
	.long push_af
	.long or_n
	.long rst_30
	.long ret_m
	.long ld_sp_hl
	.long jp_m_nn
	.long ei
	.long call_m_nn
	.long special_fd
	.long cp_n
	.long rst_38

	ALIGN



intr_table_dd:
	.long nop		# 00
	.long ld_bc_nn
	.long ld_ibc_a
	.long inc_bc
	.long inc_b
	.long dec_b
	.long ld_b_n
	.long rlca
	.long ex_af_afb
	.long add_ix_bc
	.long ld_a_ibc
	.long dec_bc
	.long inc_c
	.long dec_c
	.long ld_c_n
	.long rrca

	.long djnz_e		# 10
	.long ld_de_nn
	.long ld_ide_a
	.long inc_de
	.long inc_d
	.long dec_d
	.long ld_d_n
	.long rla
	.long jr_e
	.long add_ix_de
	.long ld_a_ide
	.long dec_de
	.long inc_e
	.long dec_e
	.long ld_e_n
	.long rra

	.long jr_nz_e		# 20
	.long ld_ix_nn
	.long ld_inn_ix
	.long inc_ix
	.long inc_ixh
	.long dec_ixh
	.long ld_ixh_n
	.long daa
	.long jr_z_e
	.long add_ix_ix
	.long ld_ix_inn
	.long dec_ix
	.long inc_ixl
	.long dec_ixl
	.long ld_ixl_n
	.long cpl

	.long jr_nc_e		# 30
	.long ld_sp_nn
	.long ld_inn_a
	.long inc_sp
	.long inc_iixd
	.long dec_iixd
	.long ld_iixd_n
	.long scf
	.long jr_c_e
	.long add_ix_sp
	.long ld_a_inn
	.long dec_sp
	.long inc_a
	.long dec_a
	.long ld_a_n
	.long ccf

	.long nop		# 40
	.long ld_b_c
	.long ld_b_d
	.long ld_b_e
	.long ld_b_ixh
	.long ld_b_ixl
	.long ld_b_iixd
	.long ld_b_a
	.long ld_c_b
	.long nop
	.long ld_c_d
	.long ld_c_e
	.long ld_c_ixh
	.long ld_c_ixl
	.long ld_c_iixd
	.long ld_c_a

	.long ld_d_b		# 50
	.long ld_d_c
	.long nop
	.long ld_d_e
	.long ld_d_ixh
	.long ld_d_ixl
	.long ld_d_iixd
	.long ld_d_a
	.long ld_e_b
	.long ld_e_c
	.long ld_e_d
	.long nop
	.long ld_e_ixh
	.long ld_e_ixl
	.long ld_e_iixd
	.long ld_e_a

	.long ld_ixh_b		# 60
	.long ld_ixh_c
	.long ld_ixh_d
	.long ld_ixh_e
	.long nop
	.long ld_ixh_ixl
	.long ld_h_iixd
	.long ld_ixh_a
	.long ld_ixl_b
	.long ld_ixl_c
	.long ld_ixl_d
	.long ld_ixl_e
	.long ld_ixl_ixh
	.long nop
	.long ld_l_iixd
	.long ld_ixl_a
	
	.long ld_iixd_b		# 70
	.long ld_iixd_c
	.long ld_iixd_d
	.long ld_iixd_e
	.long ld_iixd_h
	.long ld_iixd_l
	.long halt
	.long ld_iixd_a
	.long ld_a_b
	.long ld_a_c
	.long ld_a_d
	.long ld_a_e
	.long ld_a_ixh
	.long ld_a_ixl
	.long ld_a_iixd
	.long nop
	
	.long add_a_b		# 80
	.long add_a_c
	.long add_a_d
	.long add_a_e
	.long add_a_ixh
	.long add_a_ixl
	.long add_a_iixd
	.long add_a_a
	.long adc_a_b
	.long adc_a_c
	.long adc_a_d
	.long adc_a_e
	.long adc_a_ixh
	.long adc_a_ixl
	.long adc_a_iixd
	.long adc_a_a
	
	.long sub_b		# 90
	.long sub_c
	.long sub_d
	.long sub_e
	.long sub_ixh
	.long sub_ixl
	.long sub_iixd
	.long sub_a
	.long sbc_a_b
	.long sbc_a_c
	.long sbc_a_d
	.long sbc_a_e
	.long sbc_a_ixh
	.long sbc_a_ixl
	.long sbc_a_iixd
	.long sbc_a_a

	.long and_b		# A0
	.long and_c
	.long and_d
	.long and_e
	.long and_ixh
	.long and_ixl
	.long and_iixd
	.long and_a
	.long xor_b
	.long xor_c
	.long xor_d
	.long xor_e
	.long xor_ixh
	.long xor_ixl
	.long xor_iixd
	.long xor_a

	.long or_b		# B0
	.long or_c
	.long or_d
	.long or_e
	.long or_ixh
	.long or_ixl
	.long or_iixd
	.long or_a
	.long cp_b
	.long cp_c
	.long cp_d
	.long cp_e
	.long cp_ixh
	.long cp_ixl
	.long cp_iixd
	.long cp_a

	.long ret_nz		# C0
	.long pop_bc
	.long jp_nz_nn
	.long jp_nn
	.long call_nz_nn
	.long push_bc
	.long add_a_n
	.long rst_00
	.long ret_z
	.long ret
	.long jp_z_nn
	.long special_dd_cb
	.long call_z_nn
	.long call_nn
	.long adc_a_n
	.long rst_08

	.long ret_nc		# D0
	.long pop_de
	.long jp_nc_nn
	.long out_in_a
	.long call_nc_nn
	.long push_de
	.long sub_n
	.long rst_10
	.long ret_c
	.long exx
	.long jp_c_nn
	.long in_a_in
	.long call_c_nn
	.long special_xx
	.long sbc_a_n
	.long rst_18

	.long ret_po		# E0
	.long pop_ix
	.long jp_po_nn
	.long ex_isp_ix
	.long call_po_nn
	.long push_ix
	.long and_n
	.long rst_20
	.long ret_pe
	.long jp_ix
	.long jp_pe_nn
	.long ex_de_hl
	.long call_pe_nn
	.long special_ed
	.long xor_n
	.long rst_28

	.long ret_p		# F0
	.long pop_af
	.long jp_p_nn
	.long di
	.long call_p_nn
	.long push_af
	.long or_n
	.long rst_30
	.long ret_m
	.long ld_sp_ix
	.long jp_m_nn
	.long ei
	.long call_m_nn
	.long special_xx
	.long cp_n
	.long rst_38

	ALIGN


intr_table_fd:
	.long nop		# 00
	.long ld_bc_nn
	.long ld_ibc_a
	.long inc_bc
	.long inc_b
	.long dec_b
	.long ld_b_n
	.long rlca
	.long ex_af_afb
	.long add_iy_bc
	.long ld_a_ibc
	.long dec_bc
	.long inc_c
	.long dec_c
	.long ld_c_n
	.long rrca

	.long djnz_e		# 10
	.long ld_de_nn
	.long ld_ide_a
	.long inc_de
	.long inc_d
	.long dec_d
	.long ld_d_n
	.long rla
	.long jr_e
	.long add_iy_de
	.long ld_a_ide
	.long dec_de
	.long inc_e
	.long dec_e
	.long ld_e_n
	.long rra

	.long jr_nz_e		# 20
	.long ld_iy_nn
	.long ld_inn_iy
	.long inc_iy
	.long inc_iyh
	.long dec_iyh
	.long ld_iyh_n
	.long daa
	.long jr_z_e
	.long add_iy_iy
	.long ld_iy_inn
	.long dec_iy
	.long inc_iyl
	.long dec_iyl
	.long ld_iyl_n
	.long cpl

	.long jr_nc_e		# 30
	.long ld_sp_nn
	.long ld_inn_a
	.long inc_sp
	.long inc_iiyd
	.long dec_iiyd
	.long ld_iiyd_n
	.long scf
	.long jr_c_e
	.long add_iy_sp
	.long ld_a_inn
	.long dec_sp
	.long inc_a
	.long dec_a
	.long ld_a_n
	.long ccf

	.long nop		# 40
	.long ld_b_c
	.long ld_b_d
	.long ld_b_e
	.long ld_b_iyh
	.long ld_b_iyl
	.long ld_b_iiyd
	.long ld_b_a
	.long ld_c_b
	.long nop
	.long ld_c_d
	.long ld_c_e
	.long ld_c_iyh
	.long ld_c_iyl
	.long ld_c_iiyd
	.long ld_c_a

	.long ld_d_b		# 50
	.long ld_d_c
	.long nop
	.long ld_d_e
	.long ld_d_iyh
	.long ld_d_iyl
	.long ld_d_iiyd
	.long ld_d_a
	.long ld_e_b
	.long ld_e_c
	.long ld_e_d
	.long nop
	.long ld_e_iyh
	.long ld_e_iyl
	.long ld_e_iiyd
	.long ld_e_a

	.long ld_iyh_b		# 60
	.long ld_iyh_c
	.long ld_iyh_d
	.long ld_iyh_e
	.long nop
	.long ld_iyh_iyl
	.long ld_h_iiyd
	.long ld_iyh_a
	.long ld_iyl_b
	.long ld_iyl_c
	.long ld_iyl_d
	.long ld_iyl_e
	.long ld_iyl_iyh
	.long nop
	.long ld_l_iiyd
	.long ld_iyl_a
	
	.long ld_iiyd_b		# 70
	.long ld_iiyd_c
	.long ld_iiyd_d
	.long ld_iiyd_e
	.long ld_iiyd_h
	.long ld_iiyd_l
	.long halt
	.long ld_iiyd_a
	.long ld_a_b
	.long ld_a_c
	.long ld_a_d
	.long ld_a_e
	.long ld_a_iyh
	.long ld_a_iyl
	.long ld_a_iiyd
	.long nop
	
	.long add_a_b		# 80
	.long add_a_c
	.long add_a_d
	.long add_a_e
	.long add_a_iyh
	.long add_a_iyl
	.long add_a_iiyd
	.long add_a_a
	.long adc_a_b
	.long adc_a_c
	.long adc_a_d
	.long adc_a_e
	.long adc_a_iyh
	.long adc_a_iyl
	.long adc_a_iiyd
	.long adc_a_a
	
	.long sub_b		# 90
	.long sub_c
	.long sub_d
	.long sub_e
	.long sub_iyh
	.long sub_iyl
	.long sub_iiyd
	.long sub_a
	.long sbc_a_b
	.long sbc_a_c
	.long sbc_a_d
	.long sbc_a_e
	.long sbc_a_iyh
	.long sbc_a_iyl
	.long sbc_a_iiyd
	.long sbc_a_a

	.long and_b		# A0
	.long and_c
	.long and_d
	.long and_e
	.long and_iyh
	.long and_iyl
	.long and_iiyd
	.long and_a
	.long xor_b
	.long xor_c
	.long xor_d
	.long xor_e
	.long xor_iyh
	.long xor_iyl
	.long xor_iiyd
	.long xor_a

	.long or_b		# B0
	.long or_c
	.long or_d
	.long or_e
	.long or_iyh
	.long or_iyl
	.long or_iiyd
	.long or_a
	.long cp_b
	.long cp_c
	.long cp_d
	.long cp_e
	.long cp_iyh
	.long cp_iyl
	.long cp_iiyd
	.long cp_a

	.long ret_nz		# C0
	.long pop_bc
	.long jp_nz_nn
	.long jp_nn
	.long call_nz_nn
	.long push_bc
	.long add_a_n
	.long rst_00
	.long ret_z
	.long ret
	.long jp_z_nn
	.long special_fd_cb
	.long call_z_nn
	.long call_nn
	.long adc_a_n
	.long rst_08

	.long ret_nc		# D0
	.long pop_de
	.long jp_nc_nn
	.long out_in_a
	.long call_nc_nn
	.long push_de
	.long sub_n
	.long rst_10
	.long ret_c
	.long exx
	.long jp_c_nn
	.long in_a_in
	.long call_c_nn
	.long special_xx
	.long sbc_a_n
	.long rst_18

	.long ret_po		# E0
	.long pop_iy
	.long jp_po_nn
	.long ex_isp_iy
	.long call_po_nn
	.long push_iy
	.long and_n
	.long rst_20
	.long ret_pe
	.long jp_iy
	.long jp_pe_nn
	.long ex_de_hl
	.long call_pe_nn
	.long special_ed
	.long xor_n
	.long rst_28

	.long ret_p		# F0
	.long pop_af
	.long jp_p_nn
	.long di
	.long call_p_nn
	.long push_af
	.long or_n
	.long rst_30
	.long ret_m
	.long ld_sp_iy
	.long jp_m_nn
	.long ei
	.long call_m_nn
	.long special_xx
	.long cp_n
	.long rst_38

	ALIGN


intr_table_ed:
	.long ill_ed		# 00
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# 10
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# 20
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# 30
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long in_b_ic		# 40
	.long out_ic_b
	.long sbc_hl_bc
	.long ld_inn_bc
	.long neg
	.long retn
	.long im_0
	.long ld_i_a
	.long in_c_ic
	.long out_ic_c
	.long adc_hl_bc
	.long ld_bc_inn
	.long neg
	.long reti
	.long im_0
	.long ld_r_a

	.long in_d_ic		# 50
	.long out_ic_d
	.long sbc_hl_de
	.long ld_inn_de
	.long neg
	.long retn
	.long im_1
	.long ld_a_i
	.long in_e_ic
	.long out_ic_e
	.long adc_hl_de
	.long ld_de_inn
	.long neg
	.long retn
	.long im_2
	.long ld_a_r

	.long in_h_ic		# 60
	.long out_ic_h
	.long sbc_hl_hl
	.long ld_inn_hl_ed
	.long neg
	.long retn
	.long im_0
	.long rrd
	.long in_l_ic
	.long out_ic_l
	.long adc_hl_hl
	.long ld_hl_inn_ed
	.long neg
	.long retn
	.long im_0
	.long rld

	.long in_f_ic		# 70
	.long out_ic_0
	.long sbc_hl_sp
	.long ld_inn_sp
	.long neg
	.long retn
	.long im_1
	.long ill_ed
	.long in_a_ic
	.long out_ic_a
	.long adc_hl_sp
	.long ld_sp_inn
	.long neg
	.long retn
	.long im_2
	.long ill_ed

	.long ill_ed		# 80
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# 90
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ldi		# A0
	.long cpi
	.long ini
	.long outi
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ldd
	.long cpd
	.long ind
	.long outd
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ldir		# B0
	.long cpir
	.long inir
	.long otir
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long lddr
	.long cpdr
	.long indr
	.long otdr
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# C0
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# D0
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# E0
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	.long ill_ed		# F0
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed
	.long ill_ed

	ALIGN


intr_table_cb:
	.long rlc_b		# 00
	.long rlc_c
	.long rlc_d
	.long rlc_e
	.long rlc_h
	.long rlc_l
	.long rlc_ihl
	.long rlc_a
	.long rrc_b
	.long rrc_c
	.long rrc_d
	.long rrc_e
	.long rrc_h
	.long rrc_l
	.long rrc_ihl
	.long rrc_a

	.long rl_b		# 10
	.long rl_c
	.long rl_d
	.long rl_e
	.long rl_h
	.long rl_l
	.long rl_ihl
	.long rl_a
	.long rr_b
	.long rr_c
	.long rr_d
	.long rr_e
	.long rr_h
	.long rr_l
	.long rr_ihl
	.long rr_a

	.long sla_b		# 20
	.long sla_c
	.long sla_d
	.long sla_e
	.long sla_h
	.long sla_l
	.long sla_ihl
	.long sla_a
	.long sra_b
	.long sra_c
	.long sra_d
	.long sra_e
	.long sra_h
	.long sra_l
	.long sra_ihl
	.long sra_a

	.long sll_b		# 30
	.long sll_c
	.long sll_d
	.long sll_e
	.long sll_h
	.long sll_l
	.long sll_ihl
	.long sll_a
	.long srl_b
	.long srl_c
	.long srl_d
	.long srl_e
	.long srl_h
	.long srl_l
	.long srl_ihl
	.long srl_a

	.long bit_0_b		# 40
	.long bit_0_c
	.long bit_0_d
	.long bit_0_e
	.long bit_0_h
	.long bit_0_l
	.long bit_0_ihl
	.long bit_0_a
	.long bit_1_b
	.long bit_1_c
	.long bit_1_d
	.long bit_1_e
	.long bit_1_h
	.long bit_1_l
	.long bit_1_ihl
	.long bit_1_a

	.long bit_2_b		# 50
	.long bit_2_c
	.long bit_2_d
	.long bit_2_e
	.long bit_2_h
	.long bit_2_l
	.long bit_2_ihl
	.long bit_2_a
	.long bit_3_b
	.long bit_3_c
	.long bit_3_d
	.long bit_3_e
	.long bit_3_h
	.long bit_3_l
	.long bit_3_ihl
	.long bit_3_a

	.long bit_4_b		# 60
	.long bit_4_c
	.long bit_4_d
	.long bit_4_e
	.long bit_4_h
	.long bit_4_l
	.long bit_4_ihl
	.long bit_4_a
	.long bit_5_b
	.long bit_5_c
	.long bit_5_d
	.long bit_5_e
	.long bit_5_h
	.long bit_5_l
	.long bit_5_ihl
	.long bit_5_a

	.long bit_6_b		# 70
	.long bit_6_c
	.long bit_6_d
	.long bit_6_e
	.long bit_6_h
	.long bit_6_l
	.long bit_6_ihl
	.long bit_6_a
	.long bit_7_b
	.long bit_7_c
	.long bit_7_d
	.long bit_7_e
	.long bit_7_h
	.long bit_7_l
	.long bit_7_ihl
	.long bit_7_a

	.long res_0_b		# 80
	.long res_0_c
	.long res_0_d
	.long res_0_e
	.long res_0_h
	.long res_0_l
	.long res_0_ihl
	.long res_0_a
	.long res_1_b
	.long res_1_c
	.long res_1_d
	.long res_1_e
	.long res_1_h
	.long res_1_l
	.long res_1_ihl
	.long res_1_a

	.long res_2_b		# 90
	.long res_2_c
	.long res_2_d
	.long res_2_e
	.long res_2_h
	.long res_2_l
	.long res_2_ihl
	.long res_2_a
	.long res_3_b
	.long res_3_c
	.long res_3_d
	.long res_3_e
	.long res_3_h
	.long res_3_l
	.long res_3_ihl
	.long res_3_a

	.long res_4_b		# A0
	.long res_4_c
	.long res_4_d
	.long res_4_e
	.long res_4_h
	.long res_4_l
	.long res_4_ihl
	.long res_4_a
	.long res_5_b
	.long res_5_c
	.long res_5_d
	.long res_5_e
	.long res_5_h
	.long res_5_l
	.long res_5_ihl
	.long res_5_a

	.long res_6_b		# B0
	.long res_6_c
	.long res_6_d
	.long res_6_e
	.long res_6_h
	.long res_6_l
	.long res_6_ihl
	.long res_6_a
	.long res_7_b
	.long res_7_c
	.long res_7_d
	.long res_7_e
	.long res_7_h
	.long res_7_l
	.long res_7_ihl
	.long res_7_a

	.long set_0_b		# C0
	.long set_0_c
	.long set_0_d
	.long set_0_e
	.long set_0_h
	.long set_0_l
	.long set_0_ihl
	.long set_0_a
	.long set_1_b
	.long set_1_c
	.long set_1_d
	.long set_1_e
	.long set_1_h
	.long set_1_l
	.long set_1_ihl
	.long set_1_a

	.long set_2_b		# D0
	.long set_2_c
	.long set_2_d
	.long set_2_e
	.long set_2_h
	.long set_2_l
	.long set_2_ihl
	.long set_2_a
	.long set_3_b
	.long set_3_c
	.long set_3_d
	.long set_3_e
	.long set_3_h
	.long set_3_l
	.long set_3_ihl
	.long set_3_a

	.long set_4_b		# E0
	.long set_4_c
	.long set_4_d
	.long set_4_e
	.long set_4_h
	.long set_4_l
	.long set_4_ihl
	.long set_4_a
	.long set_5_b
	.long set_5_c
	.long set_5_d
	.long set_5_e
	.long set_5_h
	.long set_5_l
	.long set_5_ihl
	.long set_5_a

	.long set_6_b		# F0
	.long set_6_c
	.long set_6_d
	.long set_6_e
	.long set_6_h
	.long set_6_l
	.long set_6_ihl
	.long set_6_a
	.long set_7_b
	.long set_7_c
	.long set_7_d
	.long set_7_e
	.long set_7_h
	.long set_7_l
	.long set_7_ihl
	.long set_7_a

	ALIGN

intr_table_dd_fd_cb:
	.long rlc_b		# 00
	.long rlc_c
	.long rlc_d
	.long rlc_e
	.long rlc_h
	.long rlc_l
	.long rlc_id
	.long rlc_a
	.long rrc_b
	.long rrc_c
	.long rrc_d
	.long rrc_e
	.long rrc_h
	.long rrc_l
	.long rrc_id
	.long rrc_a

	.long rl_b		# 10
	.long rl_c
	.long rl_d
	.long rl_e
	.long rl_h
	.long rl_l
	.long rl_id
	.long rl_a
	.long rr_b
	.long rr_c
	.long rr_d
	.long rr_e
	.long rr_h
	.long rr_l
	.long rr_id
	.long rr_a

	.long sla_b		# 20
	.long sla_c
	.long sla_d
	.long sla_e
	.long sla_h
	.long sla_l
	.long sla_id
	.long sla_a
	.long sra_b
	.long sra_c
	.long sra_d
	.long sra_e
	.long sra_h
	.long sra_l
	.long sra_id
	.long sra_a

	.long sll_b		# 30
	.long sll_c
	.long sll_d
	.long sll_e
	.long sll_h
	.long sll_l
	.long sll_id
	.long sll_a
	.long srl_b
	.long srl_c
	.long srl_d
	.long srl_e
	.long srl_h
	.long srl_l
	.long srl_id
	.long srl_a

	.long bit_0_b		# 40
	.long bit_0_c
	.long bit_0_d
	.long bit_0_e
	.long bit_0_h
	.long bit_0_l
	.long bit_0_id
	.long bit_0_a
	.long bit_1_b
	.long bit_1_c
	.long bit_1_d
	.long bit_1_e
	.long bit_1_h
	.long bit_1_l
	.long bit_1_id
	.long bit_1_a

	.long bit_2_b		# 50
	.long bit_2_c
	.long bit_2_d
	.long bit_2_e
	.long bit_2_h
	.long bit_2_l
	.long bit_2_id
	.long bit_2_a
	.long bit_3_b
	.long bit_3_c
	.long bit_3_d
	.long bit_3_e
	.long bit_3_h
	.long bit_3_l
	.long bit_3_id
	.long bit_3_a

	.long bit_4_b		# 60
	.long bit_4_c
	.long bit_4_d
	.long bit_4_e
	.long bit_4_h
	.long bit_4_l
	.long bit_4_id
	.long bit_4_a
	.long bit_5_b
	.long bit_5_c
	.long bit_5_d
	.long bit_5_e
	.long bit_5_h
	.long bit_5_l
	.long bit_5_id
	.long bit_5_a

	.long bit_6_b		# 70
	.long bit_6_c
	.long bit_6_d
	.long bit_6_e
	.long bit_6_h
	.long bit_6_l
	.long bit_6_id
	.long bit_6_a
	.long bit_7_b
	.long bit_7_c
	.long bit_7_d
	.long bit_7_e
	.long bit_7_h
	.long bit_7_l
	.long bit_7_id
	.long bit_7_a

	.long res_0_b		# 80
	.long res_0_c
	.long res_0_d
	.long res_0_e
	.long res_0_h
	.long res_0_l
	.long res_0_id
	.long res_0_a
	.long res_1_b
	.long res_1_c
	.long res_1_d
	.long res_1_e
	.long res_1_h
	.long res_1_l
	.long res_1_id
	.long res_1_a

	.long res_2_b		# 90
	.long res_2_c
	.long res_2_d
	.long res_2_e
	.long res_2_h
	.long res_2_l
	.long res_2_id
	.long res_2_a
	.long res_3_b
	.long res_3_c
	.long res_3_d
	.long res_3_e
	.long res_3_h
	.long res_3_l
	.long res_3_id
	.long res_3_a

	.long res_4_b		# A0
	.long res_4_c
	.long res_4_d
	.long res_4_e
	.long res_4_h
	.long res_4_l
	.long res_4_id
	.long res_4_a
	.long res_5_b
	.long res_5_c
	.long res_5_d
	.long res_5_e
	.long res_5_h
	.long res_5_l
	.long res_5_id
	.long res_5_a

	.long res_6_b		# B0
	.long res_6_c
	.long res_6_d
	.long res_6_e
	.long res_6_h
	.long res_6_l
	.long res_6_id
	.long res_6_a
	.long res_7_b
	.long res_7_c
	.long res_7_d
	.long res_7_e
	.long res_7_h
	.long res_7_l
	.long res_7_id
	.long res_7_a

	.long set_0_b		# C0
	.long set_0_c
	.long set_0_d
	.long set_0_e
	.long set_0_h
	.long set_0_l
	.long set_0_id
	.long set_0_a
	.long set_1_b
	.long set_1_c
	.long set_1_d
	.long set_1_e
	.long set_1_h
	.long set_1_l
	.long set_1_id
	.long set_1_a

	.long set_2_b		# D0
	.long set_2_c
	.long set_2_d
	.long set_2_e
	.long set_2_h
	.long set_2_l
	.long set_2_id
	.long set_2_a
	.long set_3_b
	.long set_3_c
	.long set_3_d
	.long set_3_e
	.long set_3_h
	.long set_3_l
	.long set_3_id
	.long set_3_a

	.long set_4_b		# E0
	.long set_4_c
	.long set_4_d
	.long set_4_e
	.long set_4_h
	.long set_4_l
	.long set_4_id
	.long set_4_a
	.long set_5_b
	.long set_5_c
	.long set_5_d
	.long set_5_e
	.long set_5_h
	.long set_5_l
	.long set_5_id
	.long set_5_a

	.long set_6_b		# F0
	.long set_6_c
	.long set_6_d
	.long set_6_e
	.long set_6_h
	.long set_6_l
	.long set_6_id
	.long set_6_a
	.long set_7_b
	.long set_7_c
	.long set_7_d
	.long set_7_e
	.long set_7_h
	.long set_7_l
	.long set_7_id
	.long set_7_a

	ALIGN

	.text
