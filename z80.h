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

#ifndef Z80_H
#define Z80_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#include <sys/types.h>
#if defined(_BIG_ENDIAN)
#define WORDS_BIGENDIAN
#endif
#endif

#ifndef COMPARISON
#define PRNM(x) z80_ ## x
#else 
#define PRNM(x) z80x_ ## x
#endif

#define DANM(x) PRNM(proc).x

#include "z80_type.h"


#ifndef WORDS_BIGENDIAN
union dregp {
    struct { byte l, h, _b2, _b3; } s;
    struct { dbyte d, _d1; }        d;
    byte*                           p;
};
#else
union dregp {
    struct { byte _b3, _b2, h, l; } s;
    struct { dbyte _d1, d; }        d;
    byte*                           p;
};
#endif

#define NUMDREGS  13

#define PORTNUM 256

/* Do NOT change the order! */
typedef struct {
    union dregp regs[NUMDREGS];
  
    int haltstate;
    int it_mode;
    int iff1, iff2;
  
    byte *mem;

    int tc;
    int rl7;

#ifdef SPECT_MEM       /* WARNING: Do NOT change the order!!! */
    int next_scri;
    int inport_mask;
    int ula_inport;
    int ula_outport;
    int sound_sam;
    int sound_change;
    int imp_change;
    int pc_speaker;
#endif

#ifdef Z80C
    dbyte cbaddr;
#endif

} Z80;


extern Z80 PRNM(proc);

extern byte PRNM(inports)[];
extern byte PRNM(outports)[];

#define TC DANM(tc)

#define XBC   0
#define XDE   1
#define XHL   2
#define XAF   3
#define XIR   4
#define XIX   5
#define XIY   6
#define XPC   7
#define XSP   8
#define XBCBK 9
#define XDEBK 10
#define XHLBK 11
#define XAFBK 12

#define XRB    0
#define XRC    1
#define XRD    2
#define XRE    3
#define XRH    4
#define XRL    5
#define XRA    6
#define XRF    7
#define XRI    8
#define XRR    9
#define XRXH   10
#define XRXL   11
#define XRYH   12
#define XRYL   13

#if 1
#define BC  (DANM(regs)[XBC].d.d)
#define DE  (DANM(regs)[XDE].d.d)
#define HL  (DANM(regs)[XHL].d.d)
#define AF  (DANM(regs)[XAF].d.d)
#define IR  (DANM(regs)[XIR].d.d)
#define IX  (DANM(regs)[XIX].d.d)
#define IY  (DANM(regs)[XIY].d.d)
#define PC  (DANM(regs)[XPC].d.d)
#define SP  (DANM(regs)[XSP].d.d)

#define BCP (DANM(regs)[XBC].p)
#define DEP (DANM(regs)[XDE].p)
#define HLP (DANM(regs)[XHL].p)
#define PCP (DANM(regs)[XPC].p)
#define SPP (DANM(regs)[XSP].p)
#define IXP (DANM(regs)[XIX].p)
#define IYP (DANM(regs)[XIY].p)


#define RB  (DANM(regs)[XBC].s.h)
#define RC  (DANM(regs)[XBC].s.l)
#define RD  (DANM(regs)[XDE].s.h)
#define RE  (DANM(regs)[XDE].s.l)
#define RH  (DANM(regs)[XHL].s.h)
#define RL  (DANM(regs)[XHL].s.l)
#define RA  (DANM(regs)[XAF].s.h)
#define RF  (DANM(regs)[XAF].s.l)
#define RI  (DANM(regs)[XIR].s.h)
#define RR  (DANM(regs)[XIR].s.l)
#define XH  (DANM(regs)[XIX].s.h)
#define XL  (DANM(regs)[XIX].s.l)
#define YH  (DANM(regs)[XIY].s.h)
#define YL  (DANM(regs)[XIY].s.l)
#define PCH (DANM(regs)[XPC].s.h)
#define PCL (DANM(regs)[XPC].s.l)
#define SPH (DANM(regs)[XSP].s.h)
#define SPL (DANM(regs)[XSP].s.l)

#define BCBK (DANM(regs)[XBCBK].d.d)
#define DEBK (DANM(regs)[XDEBK].d.d)
#define HLBK (DANM(regs)[XHLBK].d.d)
#define AFBK (DANM(regs)[XAFBK].d.d)

#define BBK  (DANM(regs)[XBCBK].s.h)
#define CBK  (DANM(regs)[XBCBK].s.l)
#define DBK  (DANM(regs)[XDEBK].s.h)
#define EBK  (DANM(regs)[XDEBK].s.l)
#define HBK  (DANM(regs)[XHLBK].s.h)
#define LBK  (DANM(regs)[XHLBK].s.l)
#define ABK  (DANM(regs)[XAFBK].s.h)
#define FBK  (DANM(regs)[XAFBK].s.l)
#endif

#ifdef __cplusplus
extern "C" {
#endif
 
extern void PRNM(init)(void);
extern int  PRNM(step)(int ticknum);

extern void PRNM(interrupt)(int data);
extern void PRNM(nmi)(void);
extern void PRNM(reset)(void);

extern void PRNM(pushpc)(void);
extern void PRNM(local_init)(void);

#ifdef __cplusplus
}
#endif


#endif /* Z80_H */
