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

/* #define DEBUG_AUDIO */

#include "spsound.h"

#include "config.h"
#include "spperif.h"
#include "z80.h"
#include "misc.h"
#include "interf.h"

#include <stdio.h>

int bufframes = 4;

int sound_avail = 0;
int sound_on = 1;

int sound_to_autoclose = 1;
char *sound_dev_name = NULL;
int sound_sample_rate = 0;
int sound_dsp_setfrag = 1;


#ifdef HAVE_SOUND

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>

static int snd;

static int sample_size = 8;
static int channels = 1;

#define SKIPTIME 5000

#define AUTOCLOSET 5
static int autocloset;

#define REOPENT 5
static int opent = 0;
static int last_not_played;

#define SPS_OPENED        0
#define SPS_AUTOCLOSED   -1
#define SPS_BUSY         -2
#define SPS_CLOSED       -3
#define SPS_NONEXIST     -4

static int sndstate = SPS_CLOSED;

static byte open_buf[TMNUM];

static void close_snd(int normal);

static int open_generic(void)
{
    int openflags;
    int res;

    if(sndstate >= SPS_OPENED || sndstate <= SPS_NONEXIST) return 0;

    openflags = O_WRONLY;

    snd = open(sound_dev_name, openflags | O_NONBLOCK);
    if(snd < 0) {
        int errno_save;
    
        errno_save = errno;

        if(sndstate <= SPS_CLOSED || errno != EBUSY) {
            sprintf(msgbuf, "Could not open sound device '%s': %s", 
                    sound_dev_name, strerror(errno));
            put_msg(msgbuf);
        }

        if(errno_save == EBUSY) sndstate = SPS_BUSY;
        else sndstate = SPS_NONEXIST;


        opent = time(NULL);
        return 0;
    }

    res = fcntl(snd, F_SETFL, openflags);
    if(res < 0) {
        sprintf(msgbuf, "Warning: fcntl failed on sound device: %s", 
                strerror(errno));
        put_msg(msgbuf);
    }

    sndstate = SPS_OPENED;
    sound_avail = 1;

    autocloset = time(NULL);
    last_not_played = 1;

    return 1;
}

static void close_generic(void)
{
    if(sndstate >= SPS_OPENED) close(snd);
  
    sound_avail = 0;
    sndstate = SPS_CLOSED;
    opent = 0;
}

static void write_generic(int numsam)
{
    int pl_at;
    int res;

    for(pl_at = 0; pl_at != numsam; ) {

        res = write(snd, sp_sound_buf+pl_at, (size_t) (numsam-pl_at));
        if(res < 0) {
      
            /* Ignore 'Interrupted system call' errors */
            if(errno != EINTR) { 
                sprintf(msgbuf, "Error writing sound device: %s", strerror(errno));
                put_msg(msgbuf);
                close_snd(0);
                return;
            }
            else return;
        }
    
        pl_at += res;
    
        if(pl_at != numsam) {
            struct timeval waittv;    
      
#ifdef DEBUG_AUDIO
            fprintf(stderr, "rem: %i\n", numsam - pl_at); 
#endif
      
            waittv.tv_sec = 0;
            waittv.tv_usec = SKIPTIME;
            select(0, NULL, NULL, NULL, &waittv);
        }
    }
}


/* -------- Open Sound System support -------- */

#ifdef OSS_SOUND

#include <sys/soundcard.h>
#include <sys/ioctl.h>

#define VOLREDUCE 2

static int buffrag = 8;


static void close_snd(int normal)
{
    if(sndstate >= SPS_OPENED && normal) {
        int res;
    
        res = ioctl(snd,SOUND_PCM_SYNC,0);
        if(res < 0) {
            sprintf(msgbuf, "ioctl(SOUND_PCM_WRITE_SYNC, 0) failed: %s", 
                    strerror(errno));
            put_msg(msgbuf);
        }
    }

    close_generic();
}

#undef FRAG
#define FRAG(x, y) ((((x) & 0xFFFF) << 16) | ((y) & 0xFFFF))

static void open_snd(void)
{
    int parm;
    int frag;
    int res;
    int bufseg;
    int i;

    if(sound_dev_name == NULL) 
        sound_dev_name = make_string(sound_dev_name, "/dev/dsp");

    if(!sound_sample_rate) sound_sample_rate = 15625;

    if(!open_generic()) return;

    bufseg = bufframes * (TMNUM / 2) / (1 << buffrag);
    if(bufseg < 2) bufseg = 2;
    frag = FRAG(bufseg, buffrag);

    if(sound_dsp_setfrag) {
        parm = frag;  
        res = ioctl(snd,SNDCTL_DSP_SETFRAGMENT,&parm);
        if(res < 0) {
            sprintf(msgbuf, "ioctl(SNDCTL_DSP_SETFRAGMENT, %i) failed: %s", 
                    frag, strerror(errno));
            put_msg(msgbuf);
        }
        frag = parm;
    }

    parm = sample_size;
    res = ioctl(snd,SOUND_PCM_WRITE_BITS,&parm);
    if(res < 0) {
        sprintf(msgbuf, "ioctl(SOUND_PCM_WRITE_BITS, %i) failed: %s",
                sample_size, strerror(errno));
        put_msg(msgbuf);
    }
    sample_size = parm;

    parm = channels;
    res = ioctl(snd,SOUND_PCM_WRITE_CHANNELS,&parm);
    if(res < 0) {
        sprintf(msgbuf, "ioctl(SOUND_PCM_WRITE_CHANNELS, %i) failed: %s",
                channels, strerror(errno));
        put_msg(msgbuf);
    }
    channels = parm;
  
    parm = sound_sample_rate;
    res = ioctl(snd,SOUND_PCM_WRITE_RATE,&parm);
    if(res < 0) {
        sprintf(msgbuf, "ioctl(SOUND_PCM_WRITE_RATE, %i) failed: %s", 
                sound_sample_rate, strerror(errno));
        put_msg(msgbuf);
    }
    sound_sample_rate = parm;

    res = ioctl(snd,SOUND_PCM_SYNC,0);
    if(res < 0) {
        sprintf(msgbuf, "ioctl(SOUND_PCM_WRITE_SYNC, 0) failed: %s", 
                strerror(errno));
        put_msg(msgbuf);
    }

    for(i = TMNUM/2-1; i >= 0; i--) open_buf[i] = 128;
    write(snd, open_buf, TMNUM/2);

}

static void write_buf(void)
{
    write_generic(TMNUM);
}

void setbufsize(void)
{
    struct timeval waittv;    

    close_snd(1);

    waittv.tv_sec = 0;
    waittv.tv_usec = 100000;
    select(0, NULL, NULL, NULL, &waittv);
  
    open_snd();
}

#endif /* OSS_SOUND */

/* -------- Sun Sound support -------- */

#ifdef SUN_SOUND

#include <sys/audioio.h>
#include <sys/ioctl.h>

#define HAVE_SOUND_FLUSH
#ifdef HAVE_SOUND_FLUSH
#ifndef __OpenBSD__
#include <stropts.h>
#endif
#include <sys/conf.h>
#endif

#define CONVERT_TO_ULAW
#define VOLREDUCE 1

const byte lin8_ulaw[] = {
    31,   31,   31,   32,   32,   32,   32,   33, 
    33,   33,   33,   34,   34,   34,   34,   35, 
    35,   35,   35,   36,   36,   36,   36,   37, 
    37,   37,   37,   38,   38,   38,   38,   39, 
    39,   39,   39,   40,   40,   40,   40,   41, 
    41,   41,   41,   42,   42,   42,   42,   43, 
    43,   43,   43,   44,   44,   44,   44,   45, 
    45,   45,   45,   46,   46,   46,   46,   47, 
    47,   47,   47,   48,   48,   49,   49,   50, 
    50,   51,   51,   52,   52,   53,   53,   54, 
    54,   55,   55,   56,   56,   57,   57,   58, 
    58,   59,   59,   60,   60,   61,   61,   62, 
    62,   63,   63,   64,   65,   66,   67,   68, 
    69,   70,   71,   72,   73,   74,   75,   76, 
    77,   78,   79,   81,   83,   85,   87,   89, 
    91,   93,   95,   99,  103,  107,  111,  119, 
    255,  247,  239,  235,  231,  227,  223,  221, 
    219,  217,  215,  213,  211,  209,  207,  206, 
    205,  204,  203,  202,  201,  200,  199,  198, 
    197,  196,  195,  194,  193,  192,  191,  191, 
    190,  190,  189,  189,  188,  188,  187,  187, 
    186,  186,  185,  185,  184,  184,  183,  183, 
    182,  182,  181,  181,  180,  180,  179,  179, 
    178,  178,  177,  177,  176,  176,  175,  175, 
    175,  175,  174,  174,  174,  174,  173,  173, 
    173,  173,  172,  172,  172,  172,  171,  171, 
    171,  171,  170,  170,  170,  170,  169,  169, 
    169,  169,  168,  168,  168,  168,  167,  167, 
    167,  167,  166,  166,  166,  166,  165,  165, 
    165,  165,  164,  164,  164,  164,  163,  163, 
    163,  163,  162,  162,  162,  162,  161,  161, 
    161,  161,  160,  160,  160,  160,  159,  159, 
};


static int written;
static int buffernum;
static int halving = 0;
static int samplenum;

static void close_snd(int normal)
{
#ifdef HAVE_SOUND_FLUSH
    if(normal)
#ifdef __OpenBSD__
        ioctl (snd, AUDIO_FLUSH);
#else
    ioctl (snd, I_FLUSH, FLUSHW);
#endif
#endif

    close_generic();
}


static void open_snd(void)
{
    audio_info_t auinfo;
    int res;
    int i;

    if(sound_dev_name == NULL) 
        sound_dev_name = make_string(sound_dev_name, "/dev/audio");

    if(!sound_sample_rate) sound_sample_rate = 16000;

    if(!open_generic()) return;

    AUDIO_INITINFO(&auinfo);
    auinfo.play.sample_rate = sound_sample_rate;
    auinfo.play.channels = channels;
    auinfo.play.precision = sample_size;
    auinfo.play.encoding = AUDIO_ENCODING_ULAW;
  
    res = ioctl(snd, AUDIO_SETINFO, &auinfo);
  
    if(res < 0) {
        put_msg("Failed to set audio information, trying samplerate = 8000");

        sound_sample_rate = 8000;
        halving = 1;

        AUDIO_INITINFO(&auinfo);
        auinfo.play.sample_rate = sound_sample_rate;
        auinfo.play.channels = channels;
        auinfo.play.precision = sample_size;
        auinfo.play.encoding = AUDIO_ENCODING_ULAW;
    
        res = ioctl(snd, AUDIO_SETINFO, &auinfo);
    
        if(res < 0) {
            sprintf(msgbuf, "Could not set audio information: %s", 
                    strerror(errno));
            put_msg(msgbuf);
        }
    }

    written = 0;
  
    buffernum = bufframes * (TMNUM / 2);
    samplenum = TMNUM;

    if(halving) {
        buffernum /= 2;
        samplenum /= 2;
    }

    for(i = samplenum/2-1; i >= 0; i--) open_buf[i] = lin8_ulaw[128];
    write(snd, open_buf, (size_t) (samplenum/2));
    written += samplenum/2;
}

static void write_buf(void)
{
    int to_cont;
    audio_info_t auinfo;
  
    if(halving) {
        byte *sb, *sbd;
        int i;

        sb = sbd = sp_sound_buf;
        if(samplenum == TMNUM/2) sb++;

        for(i = samplenum; i; sb+=2, sbd++, i--) *sbd = *sb;
    }
  
    write_generic(samplenum);
    written += samplenum;

    if(halving) samplenum = TMNUM - samplenum;

    to_cont = 0;
    do {
        int diff;

        ioctl(snd, AUDIO_GETINFO, &auinfo);
    
        diff = written - auinfo.play.samples;

        if(diff < 0 || diff > TMNUM * 100) {
            written = auinfo.play.samples;
            to_cont = 1;
            put_msg("Major slip in writing sound device");
        }
        else if(diff <= buffernum) to_cont = 1;
        else {
            struct timeval waittv;
            int waitmsec;
      
            waitmsec =  (written - auinfo.play.samples - buffernum) * 
                1000 / auinfo.play.sample_rate;
      
            waittv.tv_sec = waitmsec / 1000;
            waittv.tv_usec = (waitmsec % 1000) * 1000;
            select(0, NULL, NULL, NULL, &waittv);
        }
    } while(!to_cont);
}

void setbufsize(void)
{
    buffernum = bufframes * (TMNUM / 2);
    if(halving) {
        buffernum /= 2;
    }
}

#endif /* SUN_SOUND */


#ifdef IRIX_SOUND

#include <dmedia/audio.h>
#include <limits.h>
#define VOLREDUCE 2

static int written   = 0;
static int buffernum = 0;
static int samplenum = 0;
static ALport audio_port = 0;

static void close_snd(int normal)
{
    if(audio_port == 0) return;
    alClosePort(audio_port);
    audio_port = 0;
    sound_avail = 0;
    sndstate = SPS_CLOSED;
    opent = 0;
}

static void open_snd(void)
{
    int i;
    ALconfig config;
    if(!sound_sample_rate) sound_sample_rate = 16000;
    
    config = alNewConfig();
    alSetSampFmt(config, AL_SAMPFMT_TWOSCOMP);
    alSetWidth(config, AL_SAMPLE_8);
    alSetChannels(config, channels); 
    alSetDevice(config, AL_DEFAULT_OUTPUT);
    audio_port = alOpenPort("spectrum", "w", config);
 
    if(audio_port == 0)
    {
        sprintf(msgbuf, "Could not open audio port, error = %d\n", oserror());
        put_msg(msgbuf);
        return;
    }

    sndstate = SPS_OPENED;
    sound_avail = 1;
    autocloset = time(NULL);
    last_not_played = 1;

    written = 0;
  
    buffernum = bufframes * (TMNUM / 2);
    samplenum = TMNUM;
    
    for(i = 0; i < samplenum/2; i++) open_buf[i] = 0;
    alWriteFrames(audio_port, open_buf, samplenum/2);
    written += samplenum/2;
}

static void write_buf(void)
{ 
    int i;
    int filled;
    int nap;
    if(audio_port == 0) return;
    for(i = 0; i < samplenum; i++) sp_sound_buf[i] -= 128;
    for(;;)
    {
        filled = alGetFilled(audio_port);
        if(filled < samplenum) break;
        nap = (filled - samplenum) * CLK_TCK / sound_sample_rate + 1;
        /*printf("filled: %d nap: %d\n", filled, nap);*/
        sginap(nap);
    }
    alWriteFrames(audio_port, sp_sound_buf, samplenum);
    written += samplenum;
}

void setbufsize(void)
{
    buffernum = bufframes * (TMNUM / 2);
}

#endif /* IRIX_SOUND */



void init_spect_sound(void)
{
    if(sound_on) open_snd();
}

#ifndef VOLREDUCE
#define VOLREDUCE 2
#endif

#define CONVU8(x) ((byte) (((x) >> VOLREDUCE) + 128))

#ifdef CONVERT_TO_ULAW
#  define CONV(x) lin8_ulaw[(int) CONVU8(x)]
#else
#  define CONV(x) CONVU8(x)
#endif

#define HIGH_PASS(hp, sv) (((hp) * 15 + (sv)) >> 4)
#define TAPESOUND(tsp)    ((*tsp) >> 4)

static void process_sound(void)
{
    static int soundhp; 
    int i;
    byte *sb;
    register int sv;

    sb = sp_sound_buf;
    if(last_not_played) {
        soundhp = *sb;
        last_not_played = 0;
    }

    if(!sp_playing_tape) {
        for(i = TMNUM; i; sb++,i--) {
            sv = *sb;
            soundhp = HIGH_PASS(soundhp, sv);
            *sb = CONV(sv - soundhp);
        }
    }
    else {
        signed char *tsp;
    
        tsp = sp_tape_sound;
        for(i = TMNUM; i; sb++,tsp++,i--) {
            sv = *sb + TAPESOUND(tsp);
            soundhp = HIGH_PASS(soundhp, sv);
            *sb = CONV(sv - soundhp);
        }
    }
}

void autoclose_sound(void)
{
    if(sound_on && sound_to_autoclose && sndstate >= SPS_CLOSED) {
#ifdef DEBUG_AUDIO
        fprintf(stderr, "Autoclosing sound\n");
#endif
        close_snd(1);
        sndstate = SPS_AUTOCLOSED;
    }
}

void play_sound(int evenframe)
{
    time_t nowt;
    int snd_change;

    if(evenframe) return;

    if(sndstate <= SPS_NONEXIST) return;

    if(!sound_on) {
        if(sndstate <= SPS_CLOSED) return;
        if(sndstate < SPS_OPENED) {
            sndstate = SPS_CLOSED;
            return;
        }
        close_snd(1);
        return;
    }

    if(sndstate == SPS_CLOSED) {
        open_snd();
        if(sndstate < SPS_OPENED) return;
    }

    nowt = time(NULL);

    snd_change = z80_proc.sound_change | sp_playing_tape;
    if(snd_change) autocloset = nowt;

    if(sound_to_autoclose && 
       (sndstate >= SPS_OPENED || sndstate <= SPS_BUSY) 
       && (nowt - autocloset > AUTOCLOSET)) {
#ifdef DEBUG_AUDIO
        fprintf(stderr, "Autoclosing sound\n");
#endif
        close_snd(1);
        sndstate = SPS_AUTOCLOSED;
        return;
    }

    if(sndstate <= SPS_BUSY) {
        if(nowt - opent < REOPENT) return;
        open_snd();
        if(sndstate < SPS_OPENED) return;
    }

    if(sndstate <= SPS_AUTOCLOSED) {
        if(snd_change) {
#ifdef DEBUG_AUDIO
            fprintf(stderr, "Autoopening sound\n");
#endif
            open_snd();
            if(sndstate < SPS_OPENED) return;
        }
        else return;
    }
  
    z80_proc.sound_change = 0;

    process_sound();

    write_buf();
}

#else /* HAVE_SOUND */

/* Dummy functions */

void setbufsize(void)
{
}

void init_spect_sound(void)
{
}

void play_sound(int evenframe)
{
    evenframe = evenframe;
}

void autoclose_sound(void)
{
}

#endif /* NO_SOUND */

