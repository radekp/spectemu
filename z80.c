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

#include "z80.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Z80 PRNM(proc);

byte PRNM(inports)[PORTNUM];
byte PRNM(outports)[PORTNUM];


#ifdef SPECT_MEM
#define NUM64KSEGS 3
#endif

#ifndef NUM64KSEGS
#define NUM64KSEGS 1
#endif

#if 1
static byte *a64kmalloc(int num64ksegs)
{
    byte *bigmem;
  
    bigmem = (byte *) malloc((unsigned) (0x10000 * (num64ksegs + 1)));
    if(bigmem == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }

    return (byte *) (( (long) bigmem & ~((long) 0xFFFF)) + 0x10000);
}
#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

static byte *a64kmalloc(int num64ksegs)
{
    int fd;
    byte *baseaddr = (byte *) 0;
    byte *bigmem;
    int size = 0x10000 * num64ksegs;
    
    fd = open("/tmp/spect_mapfile", O_CREAT | O_RDWR, 0600);
    if(fd == -1) {
        perror("open");
        exit(1);
    }
  
    lseek(fd, size-1, SEEK_SET);
    write(fd, "\0", 1);

    bigmem = (byte *) mmap((void *) baseaddr, size, PROT_READ | PROT_WRITE,
                           MAP_FIXED | MAP_SHARED, fd, 0);
    
    if((int) bigmem == -1) {
        perror("mmap");
        exit(1);
    }
    
    close(fd);
    
    return bigmem;
}


#endif


void PRNM(init)(void) 
{
    qbyte i;

    DANM(mem) = a64kmalloc(NUM64KSEGS);

    srand((unsigned int) time(NULL));
    for(i = 0; i < 0x10000; i++) DANM(mem)[i] = (byte) rand();

    for(i = 0; i < NUMDREGS; i++) {
        DANM(regs)[i].p = DANM(mem);
        DANM(regs)[i].d.d = (dbyte) rand();
    }
    for(i = 0; i < PORTNUM; i++) PRNM(inports)[i] = PRNM(outports)[i] = 0;

    PRNM(local_init)();

    return;
}

/* TODO: no interrupt immediately afer EI (not important for spectrum) */

void PRNM(nmi)(void)
{
    DANM(iff2) = DANM(iff1);
    DANM(iff1) = 0;

    DANM(haltstate) = 0;
    PRNM(pushpc)();

    PC = 0x0066;
}

/* TODO: IM 0 emulation */

void PRNM(interrupt)(int data)
{
    if(DANM(iff1)) {

        DANM(haltstate) = 0;
        DANM(iff1) = DANM(iff2) = 0;

        switch(DANM(it_mode)) {
        case 0:
            PRNM(pushpc)();
            PC = 0x0038;
            break;
        case 1:
            PRNM(pushpc)();
            PC = 0x0038;
            break;
        case 2:
            PRNM(pushpc)();
            PCL = DANM(mem)[(dbyte) (((int) RI << 8) + (data & 0xFF))];
            PCH = DANM(mem)[(dbyte) (((int) RI << 8) + (data & 0xFF) + 1)];
            break;
        }
    }
}


void PRNM(reset)(void)
{
    DANM(haltstate) = 0;
    DANM(iff1) = DANM(iff2) = 0;
    DANM(it_mode) = 0;
    RI = 0;
    RR = 0;
    PC = 0;
}
