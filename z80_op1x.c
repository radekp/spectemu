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

/* IX */

LD_RR_NN(ix, XIX, 2)

ADD_RR_RR(ix, XIX, bc, XBC, 0)
ADD_RR_RR(ix, XIX, de, XDE, 1)
ADD_RR_RR(ix, XIX, ix, XIX, 2)
ADD_RR_RR(ix, XIX, sp, XSP, 3)

INC_RR(ix, XIX, 2)

DEC_RR(ix, XIX, 2)

LD_INN_RR(ix, XIX)

LD_RR_INN(ix, XIX)

INC_R(ixh, XRXH, 4)
INC_R(ixl, XRXL, 5)

OPDEF(inc_iixd, 0x34)
{
    register dbyte addr;
    register int itmp;
    IXDGET(XIX, addr);
    itmp = (byte) (READ(addr) + 1);
    INCFLAG(itmp);
    WRITE(addr, itmp);
    TIME(19);
    ENDOP();
}

DEC_R(ixh, XRXH, 4)
DEC_R(ixl, XRXL, 5)

OPDEF(dec_iixd, 0x35)
{
    register dbyte addr;
    register int itmp;
    IXDGET(XIX, addr);
    itmp = (byte) (READ(addr) - 1);
    DECFLAG(itmp);
    WRITE(addr, itmp);
    TIME(19);
    ENDOP();
}


LD_R_N(ixh, XRXH, 4)
LD_R_N(ixl, XRXL, 5)

OPDEF(ld_iixd_n, 0x36)
{
    register dbyte addr;
    IXDGET(XIX, addr);
    WRITE(addr, MGET(XPC)); 
    RINC(XPC);
    TIME(15);
    ENDOP();
}


/* IY */

LD_RR_NN(iy, XIY, 2)

ADD_RR_RR(iy, XIY, bc, XBC, 0)
ADD_RR_RR(iy, XIY, de, XDE, 1)
ADD_RR_RR(iy, XIY, iy, XIY, 2)
ADD_RR_RR(iy, XIY, sp, XSP, 3)

INC_RR(iy, XIY, 2)

DEC_RR(iy, XIY, 2)

LD_INN_RR(iy, XIY)

LD_RR_INN(iy, XIY)

INC_R(iyh, XRYH, 4)
INC_R(iyl, XRYL, 5)

OPDEF(inc_iiyd, 0x34)
{
    register dbyte addr;
    register int itmp;
    IXDGET(XIY, addr);
    itmp = (byte) (READ(addr) + 1);
    INCFLAG(itmp);
    WRITE(addr, itmp);
    TIME(19);
    ENDOP();
}

DEC_R(iyh, XRYH, 4)
DEC_R(iyl, XRYL, 5)

OPDEF(dec_iiyd, 0x35)
{
    register dbyte addr;
    register int itmp;
    IXDGET(XIY, addr);
    itmp = (byte) (READ(addr) - 1);
    DECFLAG(itmp);
    WRITE(addr, itmp);
    TIME(19);
    ENDOP();
}


LD_R_N(iyh, XRYH, 4)
LD_R_N(iyl, XRYL, 5)

OPDEF(ld_iiyd_n, 0x36)
{
    register dbyte addr;
    IXDGET(XIY, addr);
    WRITE(addr, MGET(XPC)); 
    RINC(XPC);
    TIME(15);
    ENDOP();
}

