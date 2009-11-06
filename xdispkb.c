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

#include "xdispkb.h"
#include "xkey.h"
#include "xscr.h"
#include "spkey_p.h"

#include "ax.h"
#include "interf.h"

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int need_switch_mode = 0;

extern const unsigned long spectkey_size;
extern unsigned char spectkey[];

static GC shadow_gc;
static GC empty_gc;
static Atom delete_atom;

void (*spkb_event_processor)(void);

#define NUMROWS 4
#define NUMCOLS 10

#define MY_MIN(a,b) ((a) < (b) ? (a) : (b))
#define MY_MAX(a,b) ((a) > (b) ? (a) : (b))

static int rowsta[NUMROWS];
static int rowend[NUMROWS];

static int colsta[NUMROWS][NUMCOLS];
static int colend[NUMROWS][NUMCOLS]; 

static int pressed[NUMROWS][NUMCOLS];

#define KB_WIDTH  640
#define KB_HEIGHT 344

static int halfc_ch;
static int halfc_numhalfs;
static unsigned halfc_ctr;
static Window kbwin;
static Pixmap kbpix;
static int kb_displayed = 0;
static int kb_on_top = 0;
static int kb_mapped = 1;

static const char *kb_title = "Spectemu: Keyboard of ZX Spectrum";
static const char *kb_iconname = "Keyboard";

spkeyboard kb_mkey;


static void kb_expose(XEvent *ev, void *ptr)
{
    XExposeEvent *eev;

    ptr = ptr;

    eev = &((*ev).xexpose);

    XCopyArea(xsp_disp, kbpix, kbwin, empty_gc, 
              eev->x, eev->y, (unsigned) eev->width, (unsigned) eev->height, eev->x, eev->y);
}

static void visibility_event(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if((*ev).xvisibility.state == VisibilityUnobscured) kb_on_top = 1;
    else kb_on_top = 0;
}

static void map_event(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(ev->type == MapNotify) kb_mapped = 1;
    else {
        clear_keystates();
        kb_mapped = 0;
    }
}

static void key_call_kb(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(!kb_mapped) return;
  
    key_call(ev);
}


struct buttonstate {
    int pressed;
    int i;
    int j;
};

#define NUMBUTTONS 6

static void kb_button(XEvent *ev, void *ptr) 
{
    XButtonEvent *bev;
    int prow, pcol;
    static struct buttonstate bstate[NUMBUTTONS];
    static int inited = 0;
    int i, j;
    int but;

    ptr = ptr;

    if(!inited) {
        inited = 1;
        for(i = 0; i < NUMBUTTONS; i++) bstate[i].pressed = 0;
    }
    spkb_state_changed = 1;

    bev = &((*ev).xbutton);
    but = bev->button;
    if(but >= NUMBUTTONS) return; 
  
    if(ev->type == ButtonPress) {
        if(bstate[but].pressed) return;

        for(i = 0; i < NUMROWS; i++) 
            if(rowsta[i] <= bev->y && rowend[i] > bev->y) break;
    
        if(i == NUMROWS) return;
        prow = i;
    
        for(i = 0; i < NUMCOLS; i++)
            if(colsta[prow][i] <= bev->x && colend[prow][i] > bev->x) break;
        if(i == NUMCOLS) return;
        pcol = i;

        if(pcol < 5) {
            i = 3 - prow;
            j = pcol;
        }
        else {
            i = 4 + prow;
            j = 9 - pcol;
        }
        bstate[but].pressed = 1;
        bstate[but].i = i;
        bstate[but].j = j;

        kb_mkey[i] |= 1 << j;
    }
    else { 
        if(!bstate[but].pressed) return;

        SP_SETEMPTY(kb_mkey);
        bstate[but].pressed = 0;
        for(i = 0; i < NUMBUTTONS; i++)
            if(bstate[i].pressed) kb_mkey[bstate[i].i] |= 1 << bstate[i].j;
    }
}

void kb_refresh(void)
{
    int i, j;
    byte *km, *kmo;
    byte kmd;
    byte nkm;
    static int lastmapped = 0;
    static spkeyboard oldstate;
    int keychanged;

    if(!kb_displayed) return;
    if(!kb_mapped) {
        lastmapped = 0;
        return;
    }
    if(!lastmapped) {
        for(i = 0; i < 8; i++) oldstate[i] = 0xFF;
        lastmapped = 1;
    }


    km = spkey_state;
    kmo = oldstate;
    for(i = 8; i; i--) {
        nkm = *km;
        kmd = nkm ^ *kmo;
        if(kmd) {
            for(j = 5; j; kmd >>= 1, nkm >>= 1, j--) if(kmd & 1) {
                int row, col;
                int x, y;
                unsigned w, h;
        
                if(i > 4) {
                    row = i - 5;
                    col = 5 - j;
                }
                else {
                    row = 4 - i;
                    col = j + 4;
                }
                x = colsta[row][col];
                y = rowsta[row];
                w = colend[row][col] - x;
                h = rowend[row] - y;
                keychanged = 0;

                if(nkm & 1) {
                    if(!pressed[row][col]) {
                        pressed[row][col] = 1;
                        keychanged = 1;
                        XCopyArea(xsp_disp, kbpix, kbpix, empty_gc, x, y, w, h, x+1, y+1);
                        XFillRectangle(xsp_disp, kbpix, empty_gc, x, y, 1, h);
                        XFillRectangle(xsp_disp, kbpix, empty_gc, x, y, w, 1);
                    }
                }
                else {
                    if(pressed[row][col]) {
                        pressed[row][col] = 0;
                        keychanged = 1;
                        XCopyArea(xsp_disp, kbpix, kbpix, empty_gc, x+1, y+1, w, h, x, y);
                        XFillRectangle(xsp_disp, kbpix, shadow_gc, x+(int)w, y+1, 1, h);
                        XFillRectangle(xsp_disp, kbpix, shadow_gc, x+1, y+(int)h, w, 1);
                    }
                }
                if(keychanged) 
                    XCopyArea(xsp_disp, kbpix, kbwin, empty_gc, x, y, w+1, h+1, x, y);
            }
            *kmo = *km;
        }
        km++, kmo++;
    }
}

static void init_gethalfc(void)
{
    halfc_numhalfs = 0;
    halfc_ctr = 0;
}

static int gethalfc(void)
{
    if(!halfc_numhalfs) {
        halfc_numhalfs = 1;
        if(halfc_ctr == spectkey_size) return -1;
        halfc_ch = spectkey[halfc_ctr++];
        return (halfc_ch >> 4);
    }
    else {
        halfc_numhalfs = 0;
        return (halfc_ch & 0x0F);
    }
}

#define GETHC hc = gethalfc(); if(hc < 0) break


static unsigned long kb_coltab[10] = {0, 9, 4, 11, 7, 8, 5, 10, 14, 15};


static void misc_event(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(ev->type == ClientMessage) {
        /* If we get a client message which has the value of the delete
         * atom, it means the window manager wants us to die.
         */
        if ((*ev).xclient.data.l[0] == (unsigned char) delete_atom) {
            XUnmapWindow(xsp_disp, kbwin);
        }
    }
}

static void destroy_kb()
{
    XFreePixmap(xsp_disp, kbpix);
    XFreeGC(xsp_disp, empty_gc);
    XFreeGC(xsp_disp, shadow_gc);
    XDestroyWindow(xsp_disp, kbwin);
  
    kb_displayed = 0;
}


static int init_image()
{
    int i,j;
    XImage *kbimage;
    char *kbimage_data;
    XGCValues values;
    unsigned long valuemask;

    XSetWindowColormap(xsp_disp, kbwin, xsp_cmap);

    kbimage_data = (char *) 
        malloc((unsigned) (xsp_bpp * KB_WIDTH * KB_HEIGHT / 8));

    if(kbimage_data == NULL) {
        destroy_kb();
        put_msg("Failed to allocate memory for keyboard image");
        return 0;
    }

    kbimage = XCreateImage(xsp_disp, 
                           xsp_visual,
                           xsp_depth,
                           ZPixmap,
                           0,
                           kbimage_data,
                           KB_WIDTH,
                           KB_HEIGHT,
                           32,
                           0);

    init_gethalfc();

    i = 0;
    for(;;) {
        int hc;
        int numeq;

        hc = gethalfc();
        if(hc < 0)
            break;
        else if(hc <= 9) numeq = 1;
        else if(hc <= 13) {
            numeq = hc-7;
            GETHC;
        }
        else if(hc == 14) {
            GETHC;
            numeq = hc + 7;
            GETHC;
        }
        else {
            GETHC;
            numeq = hc;
            GETHC;
            numeq |= (hc << 4);
            GETHC;
            numeq |= (hc << 8);
            GETHC;
        }
        for(; numeq; numeq--) {
            XPutPixel(kbimage, i % KB_WIDTH, i / KB_WIDTH, 
                      xsp_colors[kb_coltab[hc]]);
            i++;
        }
    }

    valuemask = GCForeground;
    values.foreground = xsp_colors[7];
    XChangeGC(xsp_disp, shadow_gc, valuemask, &values);
    values.foreground = xsp_colors[0];
    XChangeGC(xsp_disp, empty_gc, valuemask, &values);

  
    XPutImage(xsp_disp, kbpix, empty_gc, kbimage, 
              0, 0, 0, 0, KB_WIDTH, KB_HEIGHT);
    XDestroyImage(kbimage);


    for(j = 0; j < NUMROWS; j++)
        for(i = 0; i < NUMCOLS; i++) pressed[j][i] = 0;

    return 1;
}



void kb_refresh_colormap(void)
{
    if(kb_displayed) {
        init_image();
        XCopyArea(xsp_disp, kbpix, kbwin, empty_gc,
                  0, 0, KB_WIDTH, KB_HEIGHT, 0, 0);
    }
}


int display_keyboard(void)
{
    int i, j;
    int start;
    Atom proto_atom;
    unsigned long valuemask;
    XGCValues values;

    XWMHints      *wm_hints;
    XTextProperty window_name, *wnp;
    XTextProperty icon_name, *inp;

    if(kb_displayed) {
        if(!kb_mapped /* || !kb_on_top */) XMapRaised(xsp_disp, kbwin);
        else XUnmapWindow(xsp_disp, kbwin);
        return 1;
    }
  
    kbwin = XCreateSimpleWindow(xsp_disp,
                                RootWindow(xsp_disp, xsp_scr),
                                0,0,
                                KB_WIDTH, KB_HEIGHT,
                                0, 0,
                                BlackPixel(xsp_disp, xsp_scr));

    wm_hints = XAllocWMHints();
    if(wm_hints != NULL) {
        wm_hints->input = True;
        wm_hints->initial_state = NormalState;
        wm_hints->flags = InputHint | StateHint;
    }

    wnp = &window_name;
    if(!XStringListToTextProperty((char **) &kb_title,
                                  1, wnp)) wnp = NULL;
  
    inp = &icon_name;
    if(!XStringListToTextProperty((char **) &kb_iconname, 1, inp)) inp = NULL;

    XSetWMProperties(xsp_disp, kbwin, wnp, inp, NULL, 0, 
                     NULL /* size_hints */, wm_hints, NULL /* class_hints */);
  
    if(wm_hints != NULL)    XFree((void *) wm_hints);

    /* Delete-Window-Message black magic copied from xloadimage. */
    proto_atom  = XInternAtom(xsp_disp,"WM_PROTOCOLS", False);
    delete_atom = XInternAtom(xsp_disp,"WM_DELETE_WINDOW", False);
    if ((proto_atom != None) && (delete_atom != None)) {
        XChangeProperty(xsp_disp, kbwin, proto_atom, XA_ATOM, 32,
                        PropModePrepend, (unsigned char*)&delete_atom, 1);
    }

    valuemask = GCGraphicsExposures;
    values.graphics_exposures = False;
    shadow_gc = XCreateGC(xsp_disp, kbwin, valuemask, &values);
    empty_gc = XCreateGC(xsp_disp, kbwin, valuemask, &values);

    kbpix = XCreatePixmap(xsp_disp, kbwin, KB_WIDTH, KB_HEIGHT, xsp_depth);

    if(!init_image()) return 0;
  
    start = 90;
    for(i = 0; i < NUMROWS; i++) {
        rowsta[i] = start;
        rowend[i] = start+24;
        start += 56;
    }
  
    for(j = 0; j < 3; j++) {
        switch(j) {
        case 0:
            start = 37;
            break;
        case 1:
            start = 64;
            break;
        case 2:
            start = 76;
            break;
        }

        for(i = 0; i < NUMCOLS; i++) {
            colsta[j][i] = start;
            colend[j][i] = start+38;
            start += 55;
        }
    }
  
    start = 37;
    colsta[3][0] = start;
    colend[3][0] = start+49;

    start += 66;
    for(i = 1; i < NUMCOLS - 1; i++) {
        colsta[3][i] = start;
        colend[3][i] = start+38;
        start += 55;
    }
  
    colsta[3][9] = start;
    colend[3][9] = start+66;


    aX_add_event_proc(xsp_disp, kbwin, Expose, kb_expose, 
                      ExposureMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, ButtonPress, kb_button, 
                      ButtonPressMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, ButtonRelease, kb_button, 
                      ButtonReleaseMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, ClientMessage, misc_event,
                      0, NULL);
  
    aX_add_event_proc(xsp_disp, kbwin, FocusIn, 
                      xkey_focus_change_call, FocusChangeMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, FocusOut, 
                      xkey_focus_change_call, FocusChangeMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, KeyPress,
                      key_call_kb, KeyPressMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, KeyRelease,
                      key_call_kb, KeyReleaseMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, VisibilityNotify, 
                      visibility_event, VisibilityChangeMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, ConfigureNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, CirculateNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, CreateNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, GravityNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, ReparentNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, DestroyNotify,
                      misc_event, StructureNotifyMask, NULL);

    aX_add_event_proc(xsp_disp, kbwin, MapNotify,
                      map_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, kbwin, UnmapNotify,
                      map_event, StructureNotifyMask, NULL);


    XMapWindow(xsp_disp, kbwin);
    kb_displayed = 1;

    return 1;
}
