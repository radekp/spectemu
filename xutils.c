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

/* #define DEBUG_COLOR */

#include "xscr.h"
#include "spscr.h"
#include "spscr_p.h"
#include "ax.h"
#include "xdispkb.h"
#include "spperif.h"
#include "interf.h"
#include "spconf_p.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define MAX_SEARCH_DEPTH 8


int privatemap = 0;
int fullscreen = 0;

#define RES_WIDTH 320
#define RES_HEIGHT 240

static int    current_fullscreen = 0;
static Window fs_win;

static void move_mouse_to(int x, int y)
{
    XWarpPointer(xsp_disp, None, RootWindow(xsp_disp, xsp_scr), 0, 0, 0, 0,
                 x, y);
}

#ifdef HAVE_XF86VIDMODE
#include <X11/extensions/xf86vmode.h>

static XF86VidModeModeInfo saved_mode;
static struct {
    int x, y;
} saved_view;
static int use_vidmode = 0;

Bool XF86VidModeGetModeInfo(Display *dpy, int scr, XF86VidModeModeInfo *info)
{
    XF86VidModeModeLine *l = (XF86VidModeModeLine*)
        ((char*)info + sizeof info->dotclock);
    return XF86VidModeGetModeLine(dpy, scr, &info->dotclock, l);
}



static void save_mode(void)
{
    if(use_vidmode) {
        memset(&saved_mode, 0, sizeof(saved_mode));
        XF86VidModeGetModeInfo(xsp_disp, xsp_scr, &saved_mode);
        XF86VidModeGetViewPort(xsp_disp, xsp_scr, &saved_view.x, 
                               &saved_view.y);
    }
}

static void restore_mode(void)
{
    if(use_vidmode) {
        XF86VidModeSwitchToMode(xsp_disp, xsp_scr, &saved_mode);
        XFree(saved_mode.private);
        move_mouse_to(saved_view.x, saved_view.y);
//        XF86VidModeSetViewPort(xsp_disp, xsp_scr, saved_view.x, saved_view.y);
    }
}

static int cmpmodes(const void *va, const void *vb)
{
    XF86VidModeModeInfo *a = *(XF86VidModeModeInfo**)va;
    XF86VidModeModeInfo *b = *(XF86VidModeModeInfo**)vb;
    if(a->hdisplay != b->hdisplay)
        return a->hdisplay - b->hdisplay;
    else
        return a->vdisplay - b->vdisplay;
}

static void set_best_resolution(int width, int height)
{
    XF86VidModeModeInfo **modes;
    int i;
    int nmodes;
    
    if(use_vidmode) {
        if( XF86VidModeGetAllModeLines(xsp_disp, xsp_scr, &nmodes, &modes) ) {
            qsort(modes, nmodes, sizeof *modes, cmpmodes);
            for ( i = 0; i < nmodes; ++i ) {
                if ( (modes[i]->hdisplay >= width) &&
                     (modes[i]->vdisplay >= height) )
                    break;
            }
            if ( i < nmodes )
                XF86VidModeSwitchToMode(xsp_disp, xsp_scr, modes[i]);
            
            XFree(modes);
        }
    }
}


static void check_vidmode(void)
{
    int vidmode_event, vidmode_error;

    use_vidmode = 0;
    if ( XF86VidModeQueryExtension(xsp_disp, &vidmode_event, &vidmode_error) ) 
    {
        /* FIXME:  The extension only works on local displays */
        use_vidmode = 1;
    }
}

static void get_real_resolution(int* w, int* h)
{
    if ( use_vidmode ) {
        XF86VidModeModeLine mode;
        int unused;

        if ( XF86VidModeGetModeLine(xsp_disp, xsp_scr, &unused, &mode) ) {
            *w = mode.hdisplay;
            *h = mode.vdisplay;
            return;
        }
    }
    *w = DisplayWidth(xsp_disp, xsp_scr);
    *h = DisplayHeight(xsp_disp, xsp_scr);
}


#else /* HAVE_XF86VIDMODE */

static void check_vidmode(void)
{
}

static void save_mode(void)
{
}

static void restore_mode(void)
{
}

static void set_best_resolution(int width, int height)
{
}

static void get_real_resolution(int* w, int* h)
{
    *w = DisplayWidth(xsp_disp, xsp_scr);
    *h = DisplayHeight(xsp_disp, xsp_scr);
}


#endif /* HAVE_XF86VIDMODE */

static int resize_fullscreen(void)
{
    int x, y;
    int real_w, real_h;

    if ( current_fullscreen ) {
        resize_spect_scr(1);
        XSync(xsp_disp, False);

        /* Switch resolution and cover it with the fs_win */
        move_mouse_to(0, 0);
        set_best_resolution(RES_WIDTH, RES_HEIGHT);
        move_mouse_to(0, 0);
        get_real_resolution(&real_w, &real_h);
        XResizeWindow(xsp_disp, fs_win, real_w, real_h);
        move_mouse_to(real_w/2, real_h/2);

        /* Center and reparent the drawing window */
        x = (real_w - RES_WIDTH)/2;
        y = (real_h - RES_HEIGHT)/2;
        XReparentWindow(xsp_disp, xsp_win, fs_win, x, y);
        /* FIXME: move the mouse to the old relative location */
        XRaiseWindow(xsp_disp, fs_win);
        XSync(xsp_disp, False);
    }
    return(1);
}


static void fs_map_event(XEvent *ev, void *ptr)
{
    ev = ev;
    ptr = ptr;

    /* Grab the mouse on the fullscreen window
       The event handling will know when we become active, and then
       enter fullscreen mode if we can't grab the mouse this time.
    */

    if ( XGrabPointer(xsp_disp, fs_win, True, 0, GrabModeAsync, GrabModeAsync,
                      fs_win, None, CurrentTime) != GrabSuccess ) {
        /* We lost the grab, so fall back to windowed mode */
        XUnmapWindow(xsp_disp, fs_win);
        return;
    }

    save_mode();

    current_fullscreen = 1;
    resize_fullscreen();
}

void enter_fullscreen(void)
{
    int real_w, real_h;
    /* Map the fullscreen window to blank the screen */
    get_real_resolution(&real_w, &real_h);
    XResizeWindow(xsp_disp, fs_win, real_w, real_h);
    XMapRaised(xsp_disp, fs_win);

    /* Wait until the window is mapped */
}

void leave_fullscreen(void)
{
    if ( current_fullscreen ) {
        restore_mode();

        XReparentWindow(xsp_disp, xsp_win, top_win, 0, 0);
        XUnmapWindow(xsp_disp, fs_win);
        XSync(xsp_disp, False);
        current_fullscreen = 0;
    }
}

void spscr_toggle_fullscreen(void)
{
    fullscreen = !fullscreen;
    
    if(!fullscreen)
        leave_fullscreen();
    else
        enter_fullscreen();

    sp_init_screen_mark();           /* Redraw screen */
}

void init_xutils(void)
{
    XSetWindowAttributes attr;

    check_vidmode();

    attr.override_redirect = True;
    attr.background_pixel = BlackPixel(xsp_disp, xsp_scr);
    attr.colormap = XDefaultColormap(xsp_disp, xsp_scr);

    fs_win = XCreateWindow(xsp_disp, RootWindow(xsp_disp, xsp_scr), 0, 0,
                           32, 32, 0, CopyFromParent, CopyFromParent,
                           CopyFromParent,
                           CWOverrideRedirect | CWBackPixel | CWColormap,
                           &attr);
    
    aX_add_event_proc(xsp_disp, fs_win, MapNotify,
                      fs_map_event, StructureNotifyMask, NULL);

}



static unsigned cdist(XColor *c1, XColor *c2)
{
    int dr, dg, db;

    dr = (c1->red - c2->red) >> 8;
    dg = (c1->green - c2->green) >> 8;
    db =  (c1->blue - c2->blue) >> 8;

    return dr * dr + dg * dg + db * db;
}

static int already_alloced(pixt *colors, int num, pixt pix)
{
    int i;

    for(i = 0; i < num; i++) if(colors[i] == pix) return 1;
    return 0;
}

#define MYINF ((unsigned) -1)

static int search_colors(Colormap cmap, unsigned dp)
{
    int c;
    XColor xcolor, tmpcolor;
    pixt pix;

    XColor *scrcolors = NULL;
    unsigned  ncolors = 0;
    unsigned dist, mindist;
    pixt minpix;
    int failed = 0, lfailed;
    int fails[COLORNUM];

    if(dp < 4) failed = 1;
    if(!failed) {
        if(dp <= MAX_SEARCH_DEPTH) {
            ncolors = 1 << dp;
            scrcolors = malloc(sizeof(XColor) * ncolors);

            if(scrcolors != NULL) {
                for(pix = 0; pix < ncolors; pix++) scrcolors[pix].pixel = pix;
                XQueryColors(xsp_disp, cmap, scrcolors, (int) ncolors);
            }
        }
    
    
        for(c = 0; c < COLORNUM; c++) {
            xcolor.red = spscr_crgb[c].r << 10;
            xcolor.green = spscr_crgb[c].g << 10;
            xcolor.blue = spscr_crgb[c].b << 10;
            lfailed = 0;
            if(!XAllocColor(xsp_disp, cmap, &xcolor) || 
               already_alloced(xsp_colors, c, xcolor.pixel)) {
        
                if(scrcolors != NULL) {
#ifdef DEBUG_COLOR
                    fprintf(stderr,
                            "XAllocColor failed, searching all colors...\n");
#endif
                    mindist = MYINF;
          
                    for(pix = 0; pix < ncolors; pix++) {
                        dist = cdist(&scrcolors[pix], &xcolor);
                        if(dist < mindist) {
                            tmpcolor.red = scrcolors[pix].red;
                            tmpcolor.green = scrcolors[pix].green;
                            tmpcolor.blue = scrcolors[pix].blue;
                            if(XAllocColor(xsp_disp, cmap, &tmpcolor) &&
                               !already_alloced(xsp_colors, c, tmpcolor.pixel))
                            {
                                if(mindist != MYINF) 
                                    XFreeColors(xsp_disp, cmap, &minpix, 1, 0);
                                mindist = dist;
                                minpix = pix;
                            }
                        }
                    }
          
                    if(mindist == MYINF) {
                        lfailed = 1;
#ifdef DEBUG_COLOR
                        fprintf(stderr, "Search failed\n");
#endif
                    }
                    else xsp_colors[c] = minpix;
                }
                else {
#ifdef DEBUG_COLOR  
                    fprintf(stderr, "XAllocColor failed\n");
#endif
                    lfailed = 1;
                }
            }
            else xsp_colors[c] = xcolor.pixel;
      
            if(lfailed) {
                fails[c] = 1;
                failed = 1;
            }
            else fails[c] = 0;
      
      
#ifdef DEBUG_COLOR
            tmpcolor.pixel = xsp_colors[c];
            XQueryColor(xsp_disp, cmap, &tmpcolor);
      
      
            fprintf(stderr, 
                    "Color %2i (%3i %3i %3i) -> Pixel %5lu (%3i %3i %3i)\n",
                    c, spscr_crgb[c].r, spscr_crgb[c].g, spscr_crgb[c].b,
                    xsp_colors[c], tmpcolor.red>>10, tmpcolor.green>>10, 
                    tmpcolor.blue>>10);
#endif    
      
        }
        if(failed) {
            for(c = 0; c < COLORNUM; c++) {
                if(!fails[c])
                    XFreeColors(xsp_disp, cmap, &xsp_colors[c], 1, 0);
                xsp_colors[c] = c;
            }
        }
    }
    if(scrcolors != NULL) free(scrcolors);

    if(failed) return 0;
    return 1;
}

static int firstalloced = 1;
static int currprivate;

int allocate_colors(void)
{
    int color_res;
    Colormap defcmap;

    if(!firstalloced && !currprivate) 
        XFreeColors(xsp_disp, xsp_cmap, xsp_colors, COLORNUM, 0);
  
    defcmap = DefaultColormap(xsp_disp, xsp_scr);
    color_res = search_colors(defcmap, xsp_depth);

    if((!color_res || privatemap) && 
       (xsp_visual->class == PseudoColor || xsp_visual->class == GrayScale)) {
        XColor xcolor;
        int c;
    
        if(color_res) XFreeColors(xsp_disp, defcmap, xsp_colors, COLORNUM, 0);

        if(firstalloced || !currprivate) 
            xsp_cmap = XCreateColormap(xsp_disp, xsp_win, xsp_visual, AllocAll);

        if(xsp_depth <= MAX_SEARCH_DEPTH) {   /* Is this good? */
            for(c = 0; c < (1 << xsp_depth); c++) {
                xcolor.pixel = c;
                XQueryColor(xsp_disp, defcmap, &xcolor);
                XStoreColor(xsp_disp, xsp_cmap, &xcolor);
            }
        }

        for(c = 0; c < COLORNUM; c++) {
            xcolor.pixel = xsp_colors[c];
            xcolor.red = spscr_crgb[c].r << 10;
            xcolor.green = spscr_crgb[c].g << 10;
            xcolor.blue = spscr_crgb[c].b << 10;
            xcolor.flags = DoRed | DoGreen | DoBlue;
            XStoreColor(xsp_disp, xsp_cmap, &xcolor);
        }
        color_res = 1;
        currprivate = 1;
    }
    else {
        if(!firstalloced && currprivate) XFreeColormap(xsp_disp, xsp_cmap);
        xsp_cmap = defcmap;
        currprivate = 0;
    }
    firstalloced = 0;

    return color_res;
}


void spscr_refresh_colors(void)
{
    if(!allocate_colors()) put_msg("Could not allocate colors");
  
    kb_refresh_colormap();
    XSetWindowColormap(xsp_disp, xsp_win, xsp_cmap);
    XSetWindowBackground(xsp_disp, xsp_win, xsp_colors[7]);
    XInstallColormap(xsp_disp, xsp_cmap);
    spxs_init_color();
    sp_init_screen_mark();
}

#define MAX_RES_NAME 64
#define MAXLINELEN 512

void spcf_read_xresources(void)
{
    int i;
    char resname[MAX_RES_NAME], resclass[MAX_RES_NAME];
    char line[MAXLINELEN+1];
    char *val;

    resname[0] = '.';
    resclass[0] = '.';
    resname[MAX_RES_NAME-1] = '\0';
    resclass[MAX_RES_NAME-1] = '\0';
  
    for(i = 0; spcf_options[i].option != NULL; i++) {
        strncpy(resname+1, spcf_options[i].option, MAX_RES_NAME-2);
        strncpy(resclass+1, spcf_options[i].option, MAX_RES_NAME-2);
        resclass[1] = toupper(resclass[1]);

        val = aX_get_prog_res(resname, resclass);
        if(val != NULL) spcf_set_val(i, val, NULL, 0, 0);
    }
  
    strcpy(resname+1, "color");
    strcpy(resclass+1, "Color");

    for(i = 0; i < COLORNUM; i++) {
        sprintf(resname+6, "%i", i);
        sprintf(resclass+6, "%i", i);

        val = aX_get_prog_res(resname, resclass);
        if(val != NULL) spcf_set_color(i, val, NULL, 0, 0);
    }

    strcpy(resname+1, "keys");
    strcpy(resclass+1, "Keys");
  
    val = aX_get_prog_res(resname, resclass);
    if(val != NULL) {
        char *ival, *attr;
        int l;
        int col;

        while(*val) {
            for(i = 0; val[i] && val[i] != ';'; i++);
            l = i;
            if(i > MAXLINELEN) i = MAXLINELEN;
            strncpy(line, val, (size_t) i);
            line[i] = '\0';
      
            if(spcf_parse_conf_line(line, &attr, &ival, &col) > 0) {
                int ix;
        
                ix = spcf_match_keydef(attr, "");
                if(ix >= 0) spcf_set_key(ix, ival, NULL, 0, 0);
            }
      
            if(!val[l]) break;
            val += (l+1);
        }
    }
}

