/* 
 * Copyright (C) 1998 Szeredi Miklos
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#  if !defined(HAVE_SOUND) || !defined(OSS_SOUND)
#    define NO_SOUNDCARD
#  endif
#endif

#ifndef NO_SOUNDCARD
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#define SOUND_DEV_NAME "/dev/dsp"
#else
#define SOUND_DEV_NAME "tape.out"
#endif

#include "tapefile.h"
#include "misc.h"

#define SBUFSIZE 1024
#define IMPBUFLEN 1024
#define LOWLEV 64
#define HIGLEV 192
#define LEVDIFF (HIGLEV - LOWLEV)

typedef unsigned short dbyte;
typedef unsigned char byte;

static void record_tapefile(int snd, int plen)
{
    static char soundbuf[SBUFSIZE];
    static dbyte impbuf[IMPBUFLEN];
    int i;
    long impsum;
    int impbufrem;
    int currlev;
    int osum;
    int segtype;
    int segctr;
    dbyte impl;
    dbyte *ibp = 0;
  

    impsum = 0;
    impbufrem = 0;
    currlev = 1;
    osum = 0;
  
    do {
        for(i = 0; i < SBUFSIZE; i++) {
            while(impsum < plen) {
                while(!impbufrem) {
                    impbufrem = next_imps(impbuf, IMPBUFLEN, plen * (SBUFSIZE - i));
                    if(!impbufrem) {
                        segtype = next_segment();
                        segctr = segment_pos();
                        if(segtype != SEG_VIRTUAL) printf("%4i: %s\n", segctr, seg_desc);
                        if(segtype <= SEG_END) {
                            write(snd, soundbuf, (unsigned) i);
                            return;
                        }
                    }
                    else ibp = impbuf;
                }
                currlev = !currlev;
        
                impl = *ibp;
                if(currlev) osum += impl;
                impsum += impl;
                ibp++;
                impbufrem--;
            }
            impsum -= plen;
            if(currlev) osum -= impsum;
            soundbuf[i] = (osum * LEVDIFF) / plen + LOWLEV;
            if(currlev) osum = impsum;
            else osum = 0;
        }
        write(snd, soundbuf, SBUFSIZE);
    } while(1);
}


int main(int argc, char *argv[])
{
    int snd;
    int res;
    int sound_sample_rate;
    int start_block;
    const char *sound_dev_name = SOUND_DEV_NAME;
    char *tapefile;
    int type;
    struct tape_options tapeopt;

    if(argc < 3) {
        fprintf(stderr, 
                "usage: %s sample_rate tapefile [start_block [output_file]]\n",
                argv[0]);
        exit(1);
    }
  
    sound_sample_rate = atoi(argv[1]);
    tapefile = argv[2];
  
    start_block = 0;
    if(argc > 3) start_block = atoi(argv[3]);
    if(argc > 4) sound_dev_name = argv[4];

    snd = open(sound_dev_name, O_WRONLY | O_CREAT, 0644);

    if(snd < 0) {
        fprintf(stderr, "Could not open sound device '%s': %s\n", 
                sound_dev_name, strerror(errno));
        exit(1);
    }

#ifndef NO_SOUNDCARD
    {
        int parm;

        parm = sound_sample_rate;
        res = ioctl(snd,SOUND_PCM_WRITE_RATE,&parm);
        if(res < 0) 
            fprintf(stderr, "ioctl(SOUND_PCM_WRITE_RATE, %i) failed: %s\n", 
                    sound_sample_rate, strerror(errno));
        sound_sample_rate = parm;
    
        res = ioctl(snd,SOUND_PCM_SYNC,0);
        if(res < 0) 
            fprintf(stderr, "ioctl(SOUND_PCM_WRITE_SYNC, 0) failed: %s\n", 
                    strerror(errno));
    }
#endif

    if(check_ext(tapefile, "tap")) type = TAP_TAP;
    else if(check_ext(tapefile, "tzx")) type = TAP_TZX;
    else {
        fprintf(stderr, "tapefile must have either '.tap' or '.tzx' extension\n");
        exit(1);
    }

    res = open_tapefile(tapefile, type);
    if(!res) {
        fprintf(stderr, "%s\n", seg_desc);
        exit(1);
    }
    INITTAPEOPT(tapeopt);
    tapeopt.blanknoise = 0;
    tapeopt.stoppause = 5;
    set_tapefile_options(&tapeopt);

    if(start_block) {
        res = goto_segment(start_block);
        if(!res) {
            fprintf(stderr, "Could not jump to block %i: %s\n", start_block, 
                    seg_desc);
            exit(1);
        }
    }

    fprintf(stderr, "Recording `%s' to audio output at %i sample rate\n", 
            tapefile, sound_sample_rate);

    record_tapefile(snd, 3500000 / sound_sample_rate);

    return 0;
}
