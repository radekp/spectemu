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

/*
#define DEBUG_FOCUS
#define DEBUG_KEYS
*/

#define CORRMAX 5      /* The number of frames under which key timing 
                          correction is used */

#include "xkey.h"
#include "xscr.h"

#include "spkey.h"
#include "spkey_p.h"
#include "spscr.h"
#include "xdispkb.h"
#include "spconf_p.h"

#include "ax.h"

#include <X11/keysym.h>
#include <X11/Xutil.h>

#include <stdlib.h>

static int got_focus = 0;
static qbyte change_frame = 0;

void spkey_textmode(void)
{
    XAutoRepeatOn(xsp_disp);
    XSync(xsp_disp, False);
    if(fullscreen)
        spscr_toggle_fullscreen();
}

void spkey_screenmode(void)
{
    XAutoRepeatOff(xsp_disp);
    XSync(xsp_disp, False);
}


void key_call(XEvent *ev) 
{
    int i;
    XKeyEvent *kev;
    KeySym ks, shks;

    kev = &((*ev).xkey);

    if(!got_focus) {
#ifdef DEBUG_FOCUS
        fprintf(stderr, "Got keypress in unfocused mode\n");
#endif

        XAutoRepeatOff(xsp_disp);
        got_focus = 1;
        XSync(xsp_disp, False);    
    }

    ks = XLookupKeysym(kev, 0);
    i = KS_TO_KEY(ks);
    if(change_frame < sp_int_ctr) change_frame = sp_int_ctr;

    if(i >= 0) {
        if(ev->type == KeyPress) {
            char ch;

            XLookupString(kev, &ch, 1, &ks, NULL);
            shks = XLookupKeysym(kev, 1);

            spkb_last.keysym = (unsigned) ks;
            spkb_last.shifted = (unsigned) shks;
            spkb_last.modif = kev->state;
            spkb_last.index = i;

            if(!spkey_keyfuncs()) {
                spkb_kbstate[i].state = 1;
                spkb_kbstate[i].press = (dbyte) kev->time;
                spkb_kbstate[i].frame = sp_int_ctr;
            }
            else spkb_kbstate[i].state = 0;
        }
        else if(spkb_kbstate[i].state == 1) {
            int tdiff, fdiff;
            spkb_kbstate[i].state = 0;
      
            fdiff = sp_int_ctr - spkb_kbstate[i].frame;
            tdiff = (((dbyte) kev->time) - spkb_kbstate[i].press + 10) / 20;
            if(tdiff <= 0) tdiff = 1;

            if(fdiff < CORRMAX && fdiff < tdiff) {
                qbyte eframe;
        
                if(tdiff > CORRMAX) tdiff = CORRMAX;
                spkb_kbstate[i].state = 2;
                eframe = spkb_kbstate[i].frame = tdiff;
                spkb_kbstate[i].frame = eframe;

                if(change_frame < eframe) change_frame = eframe;
        
#ifdef DEBUG_KEYS
                fprintf(stderr, 
                        "Correcting key timing from %3i to %3i frames\n",
                        fdiff, tdiff);
#endif
            }
        }
    }
}
 
static void key_call_main(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(!accept_keys) return;
  
    key_call(ev);
}


void spkb_process_events(int evenframe)
{
    if(evenframe) {
        aX_look_events();
        if(spkb_event_processor) (*spkb_event_processor)();
    }

    if(sp_int_ctr <= change_frame) spkb_state_changed = 1;

    process_keys();
    kb_refresh();
}


void xkey_focus_change_call(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(ev->type == FocusIn) {
        XAutoRepeatOff(xsp_disp);
        got_focus = 1;
#ifdef DEBUG_FOCUS
        fprintf(stderr, "Focus In (%i)\n", (*ev).xfocus.detail);
#endif
    }
    else {
        clear_keystates();
        XAutoRepeatOn(xsp_disp);
        got_focus = 0;
#ifdef DEBUG_FOCUS
        fprintf(stderr, "Focus Out (%i)\n", (*ev).xfocus.detail);
#endif
    }

    XSync(xsp_disp, False);
}



static void  close_spect_key(void)
{
    spkey_textmode();
}


static void keymap_event(XEvent *ev, void *ptr)
{
    ptr = ptr;
  
    XRefreshKeyboardMapping(&((*ev).xmapping));
}


void init_spect_key(void)
{
    clear_keystates();
    init_basekeys();

    aX_add_event_proc(xsp_disp, top_win, KeyPress,
                      key_call_main, KeyPressMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, KeyRelease,
                      key_call_main, KeyReleaseMask, NULL);
  
    aX_add_event_proc(xsp_disp, 0, MappingNotify,
                      keymap_event, 0, NULL);


    atexit(close_spect_key);

    spkb_event_processor = 0;
}
