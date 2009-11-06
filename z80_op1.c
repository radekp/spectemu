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

#ifndef NO_OPDEF
#include "z80_def.h"
#include "z80_op1.h"
#endif

OPDEF(nop, 0x00)
{
    TIME(4);
    ENDOP();
}

OPDEF(ex_af_afb, 0x08)
{
    register byte *ptmp;
    ptmp = RGETP(XAFBK);
    RSETP(XAFBK, RGETP(XAF));
    RSETP(XAF, ptmp);
    TIME(4);
    ENDOP();
}

OPDEF(djnz_e, 0x10)
{
    register int itmp;
    itmp = SGET(XRB) - 1;
    SSET(XRB, itmp);
    if(!itmp) {
        RINC(XPC);
        TIME(8);
        ENDOP();
    }
    else {
        RSET(XPC, RGET(XPC)+MGETSB(XPC)+1);
        TIME(13);
        ENDOP();
    }
}

OPDEF(jr_e, 0x18)
{
    RSET(XPC, RGET(XPC)+MGETSB(XPC)+1);
    TIME(12);
    ENDOP();
}

#define JR_CC_E(ccn, cc, n) \
OPDEF(jr_ ## ccn ## _e, 0x20+n*8)           \
{                                           \
    if(!(cc)) {                             \
        RINC(XPC);                          \
        TIME(7);                            \
        ENDOP();                            \
    }                                       \
    else {                                  \
        RSET(XPC, RGET(XPC)+MGETSB(XPC)+1); \
        TIME(12);                           \
        ENDOP();                            \
    }                                       \
}

JR_CC_E(nz, !TESTZF, 0)
JR_CC_E(z,  TESTZF,  1)
JR_CC_E(nc, !TESTCF, 2)
JR_CC_E(c,  TESTCF,  3)


#define LD_RR_NN(rrn, rr, n) \
OPDEF(ld_ ## rrn ## _nn, 0x01+n*0x10)       \
{                                           \
    register int itmp;                      \
    FETCHD(itmp);                           \
    RSETN(rr, itmp);                        \
    TIME(10);                               \
    ENDOP();                                \
}

LD_RR_NN(bc, XBC, 0)
LD_RR_NN(de, XDE, 1)
LD_RR_NN(hl, XHL, 2)
LD_RR_NN(sp, XSP, 3)

#define ADD_RR_RR(rrn1, rr1, rrn2, rr2, n) \
OPDEF(add_## rrn1 ## _ ## rrn2, 0x09+n*0x10)      \
{                                                 \
    register int itmp1, itmp2, itmpr;             \
    register int idx;                             \
    itmp1 = RGET(rr1);                            \
    itmp2 = RGET(rr2);                            \
    itmpr = (dbyte) (itmp1 + itmp2);              \
    RSETN(rr1, itmpr);                            \
    idx = DIDXCALC(itmp1, itmp2, itmpr);          \
    SETFLAGS(CF | NF | HF,                        \
             TAB(addf_tbl)[idx] & (CF | HF));     \
    TIME(11);                                     \
    ENDOP();                                      \
}

ADD_RR_RR(hl, XHL, bc, XBC, 0)
ADD_RR_RR(hl, XHL, de, XDE, 1)
ADD_RR_RR(hl, XHL, hl, XHL, 2)
ADD_RR_RR(hl, XHL, sp, XSP, 3)

#define INC_RR(rrn, rr, n) \
OPDEF(inc_ ## rrn, 0x03+n*0x10)      \
{                                    \
    RINC(rr);                        \
    TIME(6);                         \
    ENDOP();                         \
}

INC_RR(bc, XBC, 0)
INC_RR(de, XDE, 1)
INC_RR(hl, XHL, 2)
INC_RR(sp, XSP, 3)

#define DEC_RR(rrn, rr, n) \
OPDEF(dec_ ## rrn, 0x0B+n*0x10)      \
{                                    \
    RSET(rr, RGET(rr)-1);            \
    TIME(6);                         \
    ENDOP();                         \
}

DEC_RR(bc, XBC, 0)
DEC_RR(de, XDE, 1)
DEC_RR(hl, XHL, 2)
DEC_RR(sp, XSP, 3)

OPDEF(ld_ibc_a, 0x02)
{
    MSET(XBC, RGETH(XAF));
    TIME(7);
    ENDOP();
}

OPDEF(ld_ide_a, 0x12)
{
    MSET(XDE, RGETH(XAF));
    TIME(7);
    ENDOP();
}

#define LD_INN_RR(rrn, rr) \
OPDEF(ld_inn_ ## rrn, 0x22)          \
{                                    \
    register int itmp;               \
    FETCHD(itmp);                    \
    DWRITE(itmp, RGET(rr));          \
    TIME(16);                        \
    ENDOP();                         \
}

LD_INN_RR(hl, XHL)


OPDEF(ld_inn_a, 0x32)
{
    register int itmp;
    FETCHD(itmp);
    WRITE(itmp, RGETH(XAF));
    TIME(13);
    ENDOP();
}

OPDEF(ld_a_ibc, 0x0A)
{
    SSETN(XRA, MGET(XBC));
    TIME(7);
    ENDOP();
}

OPDEF(ld_a_ide, 0x1A)
{
    SSETN(XRA, MGET(XDE));
    TIME(7);
    ENDOP();
}


#define LD_RR_INN(rrn, rr) \
OPDEF(ld_ ## rrn ## _inn, 0x2A)      \
{                                    \
    register int itmp;               \
    FETCHD(itmp);                    \
    RSETN(rr, DREAD(itmp));          \
    TIME(16);                        \
    ENDOP();                         \
}

LD_RR_INN(hl, XHL)


OPDEF(ld_a_inn, 0x3A)
{
    register int itmp;
    FETCHD(itmp);
    SSETN(XRA, READ(itmp));
    TIME(13);
    ENDOP();
}


#define INCFLAG(v) \
    SETFLAGS(SF | ZF | PVF | B3F | B5F, TAB(incf_tbl)[v])

#define INC_R(rn, r, n) \
OPDEF(inc_ ## rn, 0x04+n*8)        \
{                                  \
    register int itmp;             \
    itmp = (byte) (SGET(r) + 1);   \
    INCFLAG(itmp);                 \
    SSETN(r, itmp);                \
    TIME(4);                       \
    ENDOP();                       \
}

INC_R(b, XRB, 0)
INC_R(c, XRC, 1)
INC_R(d, XRD, 2)
INC_R(e, XRE, 3)
INC_R(h, XRH, 4)
INC_R(l, XRL, 5)
INC_R(a, XRA, 7)

OPDEF(inc_ihl, 0x34)
{
    register int itmp;
    itmp = (byte) (MGET(XHL) + 1);
    INCFLAG(itmp);
    MSET(XHL, itmp);
    TIME(11);
    ENDOP();
}


#define DECFLAG(v) \
    SETFLAGS(SF | ZF | PVF | B3F | B5F, TAB(decf_tbl)[v])

#define DEC_R(rn, r, n) \
OPDEF(dec_ ## rn, 0x05+n*8)        \
{                                  \
    register int itmp;             \
    itmp = (byte) (SGET(r) - 1);   \
    DECFLAG(itmp);                 \
    SSETN(r, itmp);                \
    TIME(4);                       \
    ENDOP();                       \
}

DEC_R(b, XRB, 0)
DEC_R(c, XRC, 1)
DEC_R(d, XRD, 2)
DEC_R(e, XRE, 3)
DEC_R(h, XRH, 4)
DEC_R(l, XRL, 5)
DEC_R(a, XRA, 7)

OPDEF(dec_ihl, 0x35)
{
    register int itmp;
    itmp = (byte) (MGET(XHL) - 1);
    DECFLAG(itmp);
    MSET(XHL, itmp);
    TIME(11);
    ENDOP();
}

#define LD_R_N(rn, r, n) \
OPDEF(ld_ ## rn ## _n, 0x06+n*8)     \
{                                    \
    SSETN(r, MGET(XPC));             \
    RINC(XPC);                       \
    TIME(7);                         \
    ENDOP();                         \
}

LD_R_N(b, XRB, 0)
LD_R_N(c, XRC, 1)
LD_R_N(d, XRD, 2)
LD_R_N(e, XRE, 3)
LD_R_N(h, XRH, 4)
LD_R_N(l, XRL, 5)
LD_R_N(a, XRA, 7)


OPDEF(ld_ihl_n, 0x36)
{
    MSET(XHL, MGET(XPC));
    RINC(XPC);
    TIME(10);
    ENDOP();
}

OPDEF(rlca, 0x07)
{
    register int itmp;
    itmp = (SGET(XRA) & 0x80) >> 7;
    SETFLAGS(HF | NF | CF, itmp);
    SSET(XRA, (SGET(XRA) << 1) | itmp);
    TIME(4);
    ENDOP();
}

OPDEF(rrca, 0x0F)
{
    register int itmp;
    itmp = (SGET(XRA) & 0x01);
    SETFLAGS(HF | NF | CF, itmp);
    if(itmp) {
        SSETN(XRA, (SGET(XRA) >> 1) | 0x80);
        TIME(4);
        ENDOP();
    }
    else {
        SSETN(XRA, SGET(XRA) >> 1);
        TIME(4);
        ENDOP();
    }
}


OPDEF(rla, 0x17)
{
    register int itmp;
    itmp = SGET(XRA) & 0x80;
    SSET(XRA, (SGET(XRA) << 1) | (SGET(XRF) & CF));
    SETFLAGS(HF | NF | CF, itmp >> 7);
    ENTIME(4);
    ENDOP();
}


OPDEF(rra, 0x1F)
{
    register int itmp;
    itmp = TESTCF;
    SETFLAGS(HF | NF | CF, SGET(XRA) & 0x01);
    if(itmp) {
        SSETN(XRA, (SGET(XRA) >> 1) | 0x80);
        TIME(4);
        ENDOP();
    }
    else {
        SSETN(XRA, SGET(XRA) >> 1);
        TIME(4);
        ENDOP();
    }
}


OPDEF(daa, 0x27)
{
    register int flag;
    register int itmp;
    
    flag = SGET(XRF);
    itmp = SGET(XRA);
    
    if(!TESTNF) {
        if(flag & CF) itmp += 0x60;
        else if(itmp > 0x99) itmp += 0x60, flag |= CF;
        
        if(flag & HF) itmp += 0x06;
        else if((itmp & 0x0F) > 9) itmp += 0x06, flag |= HF;
    }
    else {
        if(flag & CF) itmp -= 0x60;
        else if(itmp > 0x99) itmp -= 0x60, flag |= CF;
        
        if(flag & HF) itmp -= 0x06;
        else if((itmp & 0x0F) > 9) itmp -= 0x06, flag |= HF;
    }
    itmp = (byte) itmp;
    SSETN(XRA, itmp);
    flag = (flag & ~(SF | ZF | PVF | B3F | B5F)) | TAB(orf_tbl)[itmp];
    SSET(XRF, flag);
    
    TIME(4);
    ENDOP();
}

OPDEF(cpl, 0x2F)
{
    SSET(XRA, ~SGET(XRA));
    SET_FL(HF | NF);
    TIME(4);
    ENDOP();
}

OPDEF(scf, 0x37)
{
    SETFLAGS(HF | NF, CF);
    TIME(4);
    ENDOP();
}

OPDEF(ccf, 0x3F)
{
    SSET(XRF, (SGET(XRF) ^ CF) & ~(NF));
    /* HF undefined */
    TIME(4);
    ENDOP();
}

#include "z80_op1x.c"
