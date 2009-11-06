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

#ifndef COMPARISON	
#define SPT(s) NAME(sp_ ## s)
#else 
#define SPT(s) NAME(spx_ ## s)
#endif

#define LOAD_DI 0x0559

#define OS 80

#define EARBIT 0x40

#define NEXT_SCRI    DAT(OS,0)
#define INPORT_MASK  DAT(OS,4)
#define ULA_INPORT   DAT(OS,8)
#define ULA_OUTPORT  DAT(OS,12)
#define SOUND_SAM    DAT(OS,16)
#define SOUND_CHANGE DAT(OS,20)
#define IMP_CHANGE   DAT(OS,24)
#define PC_SPEAKER   DAT(OS,28)
	
#define JROM(addr, label) \
	testw $0xC000, addr	; \
	jz label

#define JNROM(addr, label) \
	testw $0xC000, addr	; \
	jnz label

#define JSCR(addr, label) \
	cmpw $0x5B00, addr	; \
	jb label
	
#define JNSCR(addr, label) \
	cmpw $0x5B00, addr	; \
	jnb label

	
#define JROMB(addr, label) \
	testb $0xC0, addr	; \
	jz label

#define JNROMB(addr, label) \
	testb $0xC0, addr	; \
	jnz label

#define JSCRB(addr, label) \
	cmpb $0x5B, addr	; \
	jb label
	
#define JNSCRB(addr, label) \
	cmpb $0x5B, addr	; \
	jnb label

	
#define MARK_SCR(addr) \
	xchgw RAF, addr	; \
	btsl SCRP, SPT(scr_mark)	; \
	xchgw RAF, addr

#define MARK_SCR_VWP(addr) \
	movzwl addr, VWP	; \
	btsl VWP, SPT(scr_mark)

#define MEM_DELAY(l) \
	cmpl $0, NEXT_SCRI	; \
	jl l			; \
	cmpl $86, TC		; \
	jle l			; \
	subl $2, TC

	
#define PUTMEM_REG(l, reg, regh, regp, val, endf) \
	JSCRB(regh, l ## _scr)	; \
	movb val, (regp)	; \
	endf			; \
l ## _scr:			; \
	JROMB(regh, l ## _rom)	; \
	movb val, (regp)	; \
	MARK_SCR_VWP(reg)	; \
	MEM_DELAY(l ## _rom)	; \
	endf			; \
l ## _rom:			; \
	endf

	
#define PUTMEM_REG_NE(l, reg, regh, regp, val) \
	JROMB(regh, l ## _mid)	; \
	movb val, (regp)	; \
	JNSCRB(regh, l ## _mid)	; \
	MARK_SCR(reg)		; \
	MEM_DELAY(l ## _mid)	; \
l ## _mid:


	
#define PUTMEM_DREG_NE(l, reg, regp, val) \
	JROM(reg, l ## _mid)	; \
	movb val, (regp)	; \
	JNSCR(reg, l ## _mid)	; \
	MARK_SCR(reg)		; \
	MEM_DELAY(l ## _mid)	; \
l ## _mid:
	
		
#define MODMEMF(l, func, endf) \
	JSCRB(RH, l ## _scr)	; \
	func((HLP))		; \
	endf			; \
l ## _scr:			; \
	JROMB(RH, l ## _rom)	; \
	func((HLP))		; \
	MARK_SCR_VWP(RHL)	; \
	MEM_DELAY(l ## _rend)	; \
	endf			; \
l ## _rom:			; \
	movb (HLP), RW		; \
	func(RW)		; \
l ## _rend:			; \
	endf

	
#define PUSH_CMPX(l, rh, rl) \
	decw RSP		; \
	PUTMEM_DREG_NE(l ## 1, RSP, SPP, rh); \
	decw RSP		; \
	PUTMEM_DREG_NE(l ## 2, RSP, SPP, rl)

	
#define PUSHT(l, rh, rl, t) \
	cmpw $0x5B01, RSP	; \
	jb l ## _cmpx		; \
	decw RSP		; \
	movb rh, (SPP)		; \
	decw RSP		; \
	movb rl, (SPP)		; \
	TIME(t)			; \
l ## _cmpx:			; \
	PUSH_CMPX(l, rh, rl)	; \
	TIME(t)			; \
	
#define PUSH(l, rh, rl) \
	cmpw $0x5B01, RSP	; \
	jb l ## _cmpx		; \
	decw RSP		; \
	movb rh, (SPP)		; \
	decw RSP		; \
	movb rl, (SPP)		; \
	jmp l ## _end		; \
	ALIGN			; \
l ## _cmpx:			; \
	PUSH_CMPX(l, rh, rl)	; \
l ## _end:	


#define TICKPERLINE 224
#define SOUNDPORT 0x10
	
#define IN(l, porth, portl, rd) \
	movl INPORT_MASK, VWP	; \
	testb RW, portl		; \
	jz l ## _in_active	; \
	testb $1, portl		; \
	jnz l ## _in_ulabus	; \
	cmpl TC,IMP_CHANGE	; \
	jg l ## _in_imp		; \
	movzbl porth, VWP	; \
	movb SPT(fe_inport_high)(VWP), rd; \
	andb ULA_INPORT, rd	; \
	decl TC			; \
	jmp l ## _in_end	; \
	ALIGN			; \
l ## _in_imp:			; \
	movl $0,IMP_CHANGE	; \
	xorb $EARBIT,ULA_INPORT	; \
	movzbl porth, VWP	; \
	movb SPT(fe_inport_high)(VWP), rd; \
	andb ULA_INPORT, rd	; \
	decl TC			; \
	jmp l ## _in_end	; \
	ALIGN			; \
l ## _in_active:		; \
	movzbl portl, VWP	; \
	movb Z80(inports)(VWP), rd; \
	jmp l ## _in_end	; \
	ALIGN			; \
l ## _in_ulabus:		; \
	movl PCP, VWP		; \
	movw NEXT_SCRI, RVW	; \
	cmpw $0, RVW		; \
	jl l ## _in_ulaidle	; \
	cmpl $96, TC		; \
	jle l ## _in_ulaidle	; \
	pushl TC		; \
	shrw $2, %di		; \
	shlw $5, RVW		; \
	addw $56, RVW		; \
	subw %di, RVW		; \
	popl TC			; \
	movb (VWP), rd		; \
	jmp l ## _in_end	; \
l ## _in_ulaidle:		; \
	movb $0xFF, rd		; \
	jmp l ## _in_end	; \
	ALIGN			; \
l ## _in_end:	

#ifdef PCSPEAKER_SOUND
#define OUT_PC_SPK(l, rs) \
l ## _out_pc_spk:		; \
	inb  $0x61, RW		; \
	testb $SOUNDPORT, rs	; \
	jz l ## _out_pc_0	; \
	andb $0xFC, RW		; \
	outb RW, $0x61		; \
	jmp l ## _out_end	; \
	ALIGN			; \
l ## _out_pc_0:			; \
	orb  $0x02, RW		; \
	outb RW, $0x61		; \
	jmp l ## _out_end	; \
	ALIGN

#define CHECK_PC_SPK(l) \
	cmpl $0, PC_SPEAKER	; \
	jnz l ## _out_pc_spk
#else
#define OUT_PC_SPK(l, rs)
#define CHECK_PC_SPK(l)
#endif
	
#define OUT(l, porth, portl, rs) \
	testb $1, portl		; \
	jnz l ## _out_nonfe	; \
	movb rs, RV		; \
	xorb ULA_OUTPORT, RV	; \
	decl TC			; \
	movb rs, ULA_OUTPORT	; \
	testb $SOUNDPORT, RV	; \
	jz l ## _out_end	; \
	movl $1, SOUND_CHANGE	; \
	CHECK_PC_SPK(l)		; \
	testb $SOUNDPORT, rs	; \
	jz l ## _out_s_0	; \
	addl TC, SOUND_SAM	; \
	jmp l ## _out_end	; \
	ALIGN			; \
l ## _out_s_0:			; \
	subl TC, SOUND_SAM	; \
	jmp l ## _out_end	; \
	ALIGN			; \
	OUT_PC_SPK(l, rs)	; \
l ## _out_nonfe:		; \
	movzbl portl, VWP	; \
	movb rs, Z80(outports)(VWP); \
l ## _out_end:	

#define DI_CHECK \
	cmpw $LOAD_DI+1, RPC	; \
	je load_point		; \
di_check_end:	

load_point:
	cmpl $0, SPT(quick_load)
	je di_check_end
	movl $1, SPT(load_trapped)
	movl $1, HALTSTATE
	movl $0, TC
	jmp loop_end
	
#define SCLINE %ebx

#define INC_SCLINE \
	incl SCLINE		; \
	andl $0x03FF, SCLINE	; \
	movl SCLINE, SPT(scline)

#define SCRPTR %edi

#define SPM %ecx
#define SPM0 %cl
#define SPM1 %ch

#define SPC %ebx
#define SPC0 %bl
#define SPC1 %bh

#define SPMP %esi
#define SPCP %ebp
	
#define SPI %edx
#define SPIW %dx
#define SPI0 %dl
#define SPI1 %dh

#define SCRI %ebx
	
#define COLI %edx
#define COLI0 %dl
		

#define TRANS_SCR \
	movl (SPI),%eax		; \
	stosl

	
#define SCR_8_PIXELS(spcx, spmx, aa) \
	movb spcx,SPI1		; \
	aa			; \
	movb spmx,SPI0		; \
	shrb $2,SPI0		; \
	andb $0x3C,SPI0		; \
	TRANS_SCR		; \
	movb spmx,SPI0		; \
	shlb $2,SPI0		; \
	andb $0x3C,SPI0		; \
	TRANS_SCR

#define SHRLSPC shrl $16, SPC

#define SCR_32_PIXELS \
	movl (SPMP),SPM		; \
	movl (SPCP),SPC		; \
	SCR_8_PIXELS(SPC0, SPM0, ); \
	addl $4, SPMP		; \
	SCR_8_PIXELS(SPC1, SPM1, SHRLSPC); \
	shrl $16, SPM		; \
	SCR_8_PIXELS(SPC0, SPM0, ); \
	addl $4, SPCP		; \
	SCR_8_PIXELS(SPC1, SPM1, ); \

	
#define GET_BORDER \
	movb SPT(colors)(,%ecx,1), %cl; \
	movb %cl, %ch		; \
	movw %cx, %ax		; \
	shll $16, %eax		; \
	movw %cx, %ax

	ALIGN
.globl SPT(halfframe)
/*	.type	 SPT(halfframe),@function */
SPT(halfframe):
	movl 8(%esp), %eax
	movl %eax, linesleft
	movl 4(%esp), %eax

	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	pushl %es

	cld
	
	movl %eax, TC
	movl SPT(image), %eax
	movl %eax, scrptr

#ifndef DOS
	movw %ds, %ax
#else
	movw NAME(djsp_video_segment), %ax
#endif
	movw %ax, %es
	
	call load_regs
new_loop:	
	movl SPT(scline), %eax
	movl SPT(scri)(,%eax,4), %eax
	movl %eax, NEXT_SCRI
	
	call step_loop
	
	pushl SCLINE
	movl SPT(scline), SCLINE
	
/* store sound */

	cmpl $0, PC_SPEAKER
	jz store_sound
	
	cmpl $0, SOUND_CHANGE
	je ewloop
	movl NAME(sppc_delay), %eax
wloop:
	testl %eax, %eax
	jz ewloop
	nop
	nop
	nop
	nop
	decl %eax
	jmp wloop
ewloop:
	movb ULA_OUTPORT, %al
	jmp sound_end
	ALIGN
	
store_sound:	
	
	movb SOUND_SAM, %al
	movb %al, SPT(sound_buf)(SCLINE)

	movb ULA_OUTPORT, %al
	testb $SOUNDPORT, %al
	jz sound_0
	movb $240, SOUND_SAM
	jmp sound_end
	ALIGN
sound_0:
	movb $16, SOUND_SAM
sound_end:

/* change EAR bit, store MIC bit*/

	movb %al, SPT(fe_outport_time)(SCLINE)

	cmpl $0,IMP_CHANGE
	je no_imp_change
	xorb $EARBIT, ULA_INPORT
	
no_imp_change:
	movzbl SPT(tape_impinfo)(SCLINE), %eax
	movl %eax, IMP_CHANGE

/* Check if updating screen */
	
	cmpl $0, SPT(updating)
	jz not_to_update

	movl NEXT_SCRI, %eax
	cmpl $-1, %eax
	jl not_to_update
	je only_border

scr_update:	
	pushl %ecx
	pushl %edx
	pushl %edi

	movl SPT(coli)(,SCLINE, 4), COLI
	INC_SCLINE

	movl %eax, SCRI

	movl scrptr, SCRPTR
	movl SCRPTR, %eax
	addl $320, %eax
	movl %eax, scrptr

	movzbl ULA_OUTPORT, %ecx
	andb $0x07, %cl
	cmpb %cl, SPT(lastborder)
	jne sbrd_change
	cmpl $0, SPT(border_update)
	jne sbrd_update

	addl $0x20, SCRPTR
sbrd_no_update:
	movl COLI, %eax
	shrl $3, %eax
		
	testb $0x07, COLI0
	jz clear_cm

	movl SPT(scr_mark)+(0x2C0*4)(,%eax,4), %ecx

no_clear_cm:	
	xorl %eax, %eax
	xchgl SPT(scr_mark)(,SCRI,4), %eax

	orl %eax, %ecx
	jz skip_update

	orl %ecx, SPT(imag_mark)(,COLI,4)
	orl %ecx, SPT(imag_horiz)

	shrl $3, COLI
	btsl COLI, SPT(imag_vert)
	
/* update screen line */
	
	pushl %esi
	pushl %ebp

/* %esi is PCP and SPMP ! */
	xorw %si, %si
	movl %esi, %eax
	
	shll $5, SCRI
	shll $5, COLI

	addl SCRI, SPMP
	leal 0x5800(%eax, COLI), SPCP
				
/* Draw pixels */
	movl  $0xF, %ebx
	pushl %ecx
	subl $4, %esp

	cmpl $0, SPT(flash_state)
	jnz scr_f1
	movl SPT(scr_f0_table), SPI
	jmp scr_32_pixels
	ALIGN

scr_f1:
	movl SPT(scr_f1_table), SPI
	jmp scr_32_pixels
	ALIGN

scr_32_pixels:
	testl %ebx, %ecx
	jz skip_32_pixels
	movl %ebx, (%esp)
	SCR_32_PIXELS
	movl (%esp), %ebx
	shll $4, %ebx
	jz end_screen_line
	movl 4(%esp), %ecx
	jmp scr_32_pixels
	ALIGN
	
skip_32_pixels:
	addl $4, SPMP
	addl $4, SPCP
	addl $32, SCRPTR
	shll $4, %ebx
	jz end_screen_line
	jmp scr_32_pixels
	ALIGN

end_screen_line:
	addl $8, %esp
	
	popl %ebp
	popl %esi

skip_update:	
	
	popl %edi
	popl %edx
	popl %ecx
	
	jmp update_end
		
	ALIGN		


sbrd_change:	
	movl $2, SPT(border_update)
	xchgb %cl, SPT(lastborder)
sbrd_update:	
	GET_BORDER
	
/* Draw border */
	xorl %ecx, %ecx
	movb $8, %cl
	rep
	stosl
	addl $0x100, SCRPTR
	movb $8, %cl
	rep
	stosl
	subl $0x120, SCRPTR
	jmp sbrd_no_update

	ALIGN
	
clear_cm:
	xorl %ecx, %ecx
	xchgl SPT(scr_mark)+(0x2C0*4)(,%eax,4), %ecx
	testl %ecx, %ecx
	jz no_clear_cm

	leal SPT(scr_mark)(,SCRI,4), %eax
	
	orl %ecx, (%eax)
	orl %ecx, 0x20(%eax)
	orl %ecx, 0x40(%eax)
	orl %ecx, 0x60(%eax)
	addl $0x80, %eax
	orl %ecx, (%eax)
	orl %ecx, 0x20(%eax)
	orl %ecx, 0x40(%eax)
	orl %ecx, 0x60(%eax)

	jmp no_clear_cm
	ALIGN

	
only_border:
	INC_SCLINE
	
	pushl %edi
	movl %ecx, %ebx

	movzbl ULA_OUTPORT, %ecx
	andb $0x07, %cl
	cmpb %cl, SPT(lastborder)
	je brd_no_change
	movl $2, SPT(border_update)
	xchgb %cl, SPT(lastborder)
brd_no_change:
	cmpl $0, SPT(border_update)
	je brd_no_update
	
	GET_BORDER
	
	movl scrptr, SCRPTR
	movl $80, %ecx
	rep
	stosl

brd_no_update:	
	addl $320, scrptr
	
	movl %ebx, %ecx
	popl %edi
	jmp update_end

	ALIGN
not_to_update:		
	INC_SCLINE

update_end:			
	popl SCLINE
	decl linesleft
	jz chunk_end
	addl $TICKPERLINE, TC
	jmp new_loop
	ALIGN

chunk_end:
	call store_regs
	movl TC, %eax

	popl %es
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret
	ALIGN
	
	.data
linesleft:
	.long 0
scrptr:
	.long 0
	.text
