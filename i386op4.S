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

/* instructions from 0xC0 to 0xFF */

ret_cc:
	POPD(RPC)
	TIME(11)

ret:
	POPD(RPC)
	TIME(10)

ret_nz:
	JNCOND(ZF, ret_cc)
	TIME(5)

ret_z:
	JCOND(ZF, ret_cc)
	TIME(5)

ret_nc:
	JNCOND(CF, ret_cc)
	TIME(5)

ret_c:
	JCOND(CF, ret_cc)
	TIME(5)

ret_po:
	JNCOND(PVF, ret_cc)
	TIME(5)

ret_pe:
	JCOND(PVF, ret_cc)
	TIME(5)

ret_p:
	JNCOND(SF, ret_cc)
	TIME(5)

ret_m:
	JCOND(SF, ret_cc)
	TIME(5)

pop_bc:
	POP(RB, RC)
	TIME(10)

pop_de:
	POPD(MDE)
	TIME(10)
	
pop_hl:
	POP(RH, RL)
	TIME(10)

pop_ix:
	POPD(MIX)
	TIMEX(14)
	
pop_iy:
	POPD(MIY)
	TIMEX(14)
	
pop_af:	
	POP(RA, RF)
	TIME(10)

exx:
	xchgw BBC, RBC
	movw MDE, RVW
	xchgw BDE, RVW
	movw RVW, MDE
	xchgw BHL, RHL
	TIME(4)

jp_hl:	
	movw RHL, RPC
	TIME(4)

jp_ix:	
	movw MIX, RPC
	TIMEX(8)

jp_iy:	
	movw MIY, RPC
	TIMEX(8)

	
ld_sp_hl:
	movw RHL, RSP
	TIME(6)

ld_sp_ix:
	movw MIX, RSP
	TIMEX(10)

ld_sp_iy:
	movw MIY, RSP
	TIMEX(10)

jp_cc_nn:
jp_nn:
	DFETCH
	movw RVW, RPC
	TIME(10)

jp_nz_nn:
	JNCOND(ZF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_z_nn:
	JCOND(ZF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_nc_nn:
	JNCOND(CF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_c_nn:
	JCOND(CF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_po_nn:
	JNCOND(PVF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_pe_nn:
	JCOND(PVF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_p_nn:
	JNCOND(SF, jp_cc_nn)
	DINCPC
	TIME(10)

jp_m_nn:
	JCOND(SF, jp_cc_nn)
	DINCPC
	TIME(10)


out_in_a:
	OUT(out_in_a, RA, (PCP), RA)
	INCPC
	TIME(11)

in_a_in:
	IN(in_a_in, RA, (PCP), RA)
	INCPC
	TIME(11)

ex_isp_hl:
	POP(RV, RW)
	PUSH(ex_isp_hl, RH, RL)
	movw RVW, RHL
	TIME(19)

ex_isp_ix:
	POP(RV, RW)
	xchgw MIX, RHL
	PUSH(ex_isp_ix, RH, RL)
	movw RVW, RHL
	xchgw MIX, RHL	
	TIMEX(23)

ex_isp_iy:
	POP(RV, RW)
	xchgw MIY, RHL
	PUSH(ex_isp_iy, RH, RL)
	movw RVW, RHL
	xchgw MIY, RHL	
	TIMEX(23)
	
ex_de_hl:
	xchgw MDE, RHL
	TIME(4)

di:
	movw $0, IFF1
	movw $0, IFF2
	DI_CHECK
	TIME(4)

ei:	
	movw $1, IFF1
	movw $1, IFF2
	TIME(4)

call_cc_nn:
call_nn:
	DFETCH
	xchgw RPC, RHL
	PUSH(call_nn, RH, RL)
	xchgw RPC, RHL
	movw RVW, RPC
	TIME(17)

call_nz_nn:
	JNCOND(ZF, call_cc_nn)
	DINCPC
	TIME(10)

call_z_nn:
	JCOND(ZF, call_cc_nn)
	DINCPC
	TIME(10)

call_nc_nn:
	JNCOND(CF, call_cc_nn)
	DINCPC
	TIME(10)

call_c_nn:
	JCOND(CF, call_cc_nn)
	DINCPC
	TIME(10)

call_po_nn:
	JNCOND(PVF, call_cc_nn)
	DINCPC
	TIME(10)

call_pe_nn:
	JCOND(PVF, call_cc_nn)
	DINCPC
	TIME(10)

call_p_nn:
	JNCOND(SF, call_cc_nn)
	DINCPC
	TIME(10)

call_m_nn:
	JCOND(SF, call_cc_nn)
	DINCPC
	TIME(10)

	
push_bc:
	PUSH(push_bc, RB, RC)
	TIME(11)

push_de:
	PUSHD(push_de, MDE)
	TIME(11)
	
push_hl:
	PUSH(push_hl, RH, RL)
	TIME(11)

push_ix:
	movw MIX, RVW
	PUSH(push_ix, RV, RW)
	TIMEX(15)

push_iy:
	movw MIY, RVW
	PUSH(push_iy, RV, RW)
	TIMEX(15)

	
push_af:	
	PUSH(push_af, RA, RF)
	TIME(11)


add_a_n:
	ADD_A_R((PCP))
	INCPC
	TIME(7)

adc_a_n:
	ADC_A_R((PCP))
	INCPC
	TIME(7)

sub_n:
	SUB_R((PCP))
	INCPC
	TIME(7)

sbc_a_n:
	SBC_A_R((PCP))
	INCPC
	TIME(7)

and_n:
	AND_R((PCP))
	INCPC
	TIME(7)

xor_n:
	XOR_R((PCP))
	INCPC
	TIME(7)

or_n:
	OR_R((PCP))
	INCPC
	TIME(7)

cp_n:
	CP_R((PCP))
	INCPC
	TIME(7)

	
#define RST_NN(n) \
	movw RPC, RVW		; \
	movw $0x ## n, RPC	; \
	jmp rst_nn		; \
	ALIGN
	
rst_nn:
	PUSH(rst_nn, RV, RW)
	TIME(11)
		
rst_00:
	RST_NN(00)

rst_08:
	RST_NN(08)

rst_10:
	RST_NN(10)

rst_18:
	RST_NN(18)

rst_20:
	RST_NN(20)

rst_28:
	RST_NN(28)

rst_30:
	RST_NN(30)

rst_38:
	RST_NN(38)
