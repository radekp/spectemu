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

#include "config.h"

#include "spperif.h"
#include "z80.h"

#include "spmain.h"
#include "sptiming.h"
#include "spscr.h"
#include "spkey.h"
#include "sptape.h"
#include "spsound.h"
#include "snapshot.h"
#include "spver.h"

#include "spconf.h"

#include "interf.h"
#include "misc.h"

#include "spjoy.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifndef DOS
#include <unistd.h>
#endif

#ifdef PCSPEAKER_SOUND
#include <sys/io.h>
#endif

int endofsingle;

int sp_nosync = 0;

int showframe = 2;
int load_immed = 0;

qbyte sp_int_ctr = 0;

int sppc_delay = 0;
int sppc_perm = 0;
int sppc_use = 0;

extern unsigned char load_z80[];
extern unsigned long load_z80_size;

#define SHOW_OFFS 1

static void update(void)
{
    update_screen();
    sp_border_update >>= 1;
    sp_imag_vert = sp_imag_horiz = 0;
}

static void run_singlemode(void)
{
    int t = 0;
    int evenframe, halfsec, updateframe;

    sp_int_ctr = 0;
    endofsingle = 0;

    spti_reset();

    while(!endofsingle) {

        if(sp_paused) {
            autoclose_sound();
            while(sp_paused) {

                /* go on maintaining keyboard and joystick status */
                joystick_update();
                spkb_process_events(1);

                spti_sleep(SKIPTIME);
                translate_screen();
                update();
            }
            spti_reset();
        }

        halfsec = !(sp_int_ctr % 25);
        evenframe = !(sp_int_ctr & 1);
    
        if(screen_visible) updateframe = sp_nosync ? halfsec : 
            !((sp_int_ctr+SHOW_OFFS) % showframe);
        else updateframe = 0;

        if(halfsec) {
            sp_flash_state = ~sp_flash_state;
            flash_change();
        }

        if(evenframe) {
            play_tape();
            sp_scline = 0;
        }

        joystick_update(); 
        spkb_process_events(evenframe);
        DANM(pc_speaker) = (sound_on && sppc_use);

        sp_updating = updateframe;
    
        t += CHKTICK;
        t = sp_halfframe(t, HALFFRAME);
        if(SPNM(load_trapped)) {
            SPNM(load_trapped) = 0;
            DANM(haltstate) = 0;
            qload();
        }

        z80_interrupt(0xFF);
        sp_int_ctr++;

        if(!evenframe) rec_tape();

        if(!sp_nosync) {
            if(!sound_avail) spti_wait();

            if(updateframe) update();
            if(!sppc_use) play_sound(evenframe);
            else z80_proc.sound_change = 0;
        }
        else if(updateframe) update();
    }
}


void check_params(int argc, char *argv[])
{
    spcf_read_config();
    spcf_read_xresources();
    spcf_read_command_line(argc, argv);
}

static void init_load()
{
    if(load_immed)
        snsh_z80_load_intern(load_z80, load_z80_size);

    if(spcf_init_snapshot != NULL) {
        sprintf(msgbuf, "Loading snapshot '%s'", spcf_init_snapshot);
        put_msg(msgbuf);
        load_snapshot_file_type(spcf_init_snapshot, spcf_init_snapshot_type);
        free_string(spcf_init_snapshot);
    }
  
    if(spcf_init_tapefile != NULL) {
        sprintf(msgbuf, "Loading tape '%s'", spcf_init_tapefile);
        put_msg(msgbuf);
        start_play_file_type(spcf_init_tapefile, 0, spcf_init_tapefile_type);
        if(!load_immed) pause_play();
        free_string(spcf_init_tapefile);
    }
}

static void print_copyright(void)
{
    sprintf(msgbuf, 
            "Welcome to SPECTEMU version %s/%s (C) Szeredi Miklos 1996-2004", 
            SPECTEMU_VERSION, SPECTEMU_TYPE);
    put_msg(msgbuf);

    put_msg("This program comes with NO WARRANTY, see the file COPYING "
            "for details");
    put_msg("Press Ctrl-h for help");
}

void start_spectemu()
{
    spti_init();
    init_spect_scr();

    if(sppc_use && !sppc_perm) {
        sppc_use = 0;
        put_msg("Can't use PC speaker sound");
    }

    if(!sppc_use) init_spect_sound();

    /* If no sound is available through a sound card, but PC speaker sound
       is available, then use it. */

    if(!sppc_use && sppc_perm && sound_on && !sound_avail ) {
        put_msg("Using PC speaker for sound");
        sppc_use = 1;
    }

    init_spect_key();

    joystick_open();

    print_copyright();
    init_load();
  
    run_singlemode();
}

void spma_lose_privs(void)
{
#ifndef DOS
    setuid(getuid());
    setgid(getgid());
#endif
}

void spma_init_privileged(void)
{
#ifdef PCSPEAKER_SOUND
    if(ioperm(0x61, 1, 1) == -1) sppc_perm = 0;
    else sppc_perm = 1;
#endif
}
