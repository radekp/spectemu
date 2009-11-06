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

#include "spscr.h"
#include "spscr_p.h"
#include "xscr.h"
#include "xkey.h"
#include "spkey_p.h"

#include "ax.h"
#include "misc.h"
#include "interf.h"

#include "run.xbm"
#include "pause.xbm"
#include "mask.xbm"

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

int scrmul = 0;
int use_shm = 1;
int pause_on_iconify = 0;

int small_screen; /* DUMMY */
int vga_pause_bg; /* DUMMY */


static int wwidth;
static int wheight;

static int shm_avail;
static int shm_first_check;

int xscr_delayed_expose = 1;

static int exp_xmin, exp_xmax, exp_ymin, exp_ymax;
static int exp_need = 0;

static Pixmap runicon, pauseicon, maskicon;
static XWMHints      *wm_hints;


#define SCRMULMAX 4


#define TV_WIDTH   320
#define TV_HEIGHT  256

#define WIDTH      256
#define HEIGHT     192

#define X_OFF      ((TV_WIDTH - WIDTH) / 2)
#define Y_OFF      ((TV_HEIGHT - HEIGHT) / 2)

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

Window xsp_win;
Window top_win;
Display *xsp_disp;
int xsp_scr;
Colormap xsp_cmap;
int xsp_bpp;
Visual *xsp_visual;
unsigned xsp_depth;


static XImage *imag;

static int immed_image;
static Atom delete_atom;
static GC xsp_gc;

pixt xsp_colors[16];

static char *idp;

#ifdef HAVE_MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

static XShmSegmentInfo shminfo;
static int is_shm_image;
#endif


static int bits_per_pixel(unsigned dp)
{
    int count;
    XPixmapFormatValues *fvp;
    int i;

    fvp = XListPixmapFormats(xsp_disp, &count);
    for(i = 0; i < count; i++) 
        if(fvp[i].depth == (int) dp) return fvp[i].bits_per_pixel;

    if(dp <= 8) return 8;
    if(dp <= 16) return 16;
    if(dp <= 32) return 32;
    return 64;
}


static void translate_image(void)
{
    if(xsp_bpp == 8 && scrmul == 2) {

        int i, j;
        char *zip, *rip1, *rip2;
    
        zip = sp_image;
        rip1 = imag->data;
        rip2 = imag->data + TV_WIDTH * 2;
    
        for(i = 0; i < TV_HEIGHT; i++) {

            if(sp_border_update || 
               (i >= Y_OFF && i < TV_HEIGHT - Y_OFF && 
                sp_imag_mark[i - Y_OFF])) {

                if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF) 
                    sp_imag_mark[i - Y_OFF] = 0;
        
#ifdef I386_ASM
                for(j = TV_WIDTH / 4; j; j--) {
                    asm volatile(
                        "movl (%2), %%eax\n\t"
                        "movb %%ah, %%dl\n\t"
                        "movb %%ah, %%dh\n\t"
                        "shll $16, %%edx\n\t"
                        "movb %%al, %%dl\n\t"
                        "movb %%al, %%dh\n\t"
                        "movl %%edx, (%0)\n\t"
                        "addl $4, %2\n\t"
                        "movl %%edx, (%1)\n\t"
                       
                        "shrl $16, %%eax\n\t"
                        "movb %%ah, %%dl\n\t"
                        "movb %%ah, %%dh\n\t"
                        "addl $8, %1\n\t"
                        "shll $16, %%edx\n\t"
                        "movb %%al, %%dl\n\t"
                        "movb %%al, %%dh\n\t"
                        "movl %%edx, 4(%0)\n\t"
                        "movl %%edx, -4(%1)\n\t"
                        "addl $8, %0\n\t"
                        :
                        "=r" (rip1), "=r" (rip2), 
                        "=r" (zip)
                        :"0" (rip1), "1" (rip2), 
                        "2" (zip)
                        :"dx", "ax");
                }
#else
                for(j = TV_WIDTH; j; j--) {
                    *rip1++ = *rip2++ = *zip;
                    *rip1++ = *rip2++ = *zip;
                    zip++;
                }
#endif
                rip1 += TV_WIDTH * 2;
                rip2 += TV_WIDTH * 2;
            }
            else {
                rip1 += 4 * TV_WIDTH;
                rip2 += 4 * TV_WIDTH;
                zip += TV_WIDTH;
            }
        }
        return;
    }

    if(xsp_bpp == 8) {

        int i, j;
        char k;
        char *zip;
        char *rip1, *rip2, *rip3, *rip4;

        zip = sp_image;
        rip1 = (char *) imag->data;

        switch (scrmul) {
        case 3:
            rip2 = rip1 + 3 * TV_WIDTH;
            rip3 = rip2 + 3 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = *zip++;
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                    }
                    rip1 += 6 * TV_WIDTH;  rip2 += 6 * TV_WIDTH;
                    rip3 += 6 * TV_WIDTH;
                }
                else {
                    rip1 += 9 * TV_WIDTH;  rip2 += 9 * TV_WIDTH;
                    rip3 += 9 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 4:
            rip2 = rip1 + 4 * TV_WIDTH;
            rip3 = rip2 + 4 * TV_WIDTH;
            rip4 = rip3 + 4 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = *zip++;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                    }
                    rip1 += 12 * TV_WIDTH;  rip2 += 12 * TV_WIDTH;
                    rip3 += 12 * TV_WIDTH;  rip4 += 12 * TV_WIDTH;
                }
                else {
                    rip1 += 16 * TV_WIDTH;  rip2 += 16 * TV_WIDTH;
                    rip3 += 16 * TV_WIDTH;  rip4 += 16 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;
        }
    }

    if(xsp_bpp == 16) {

        int i, j;
        dbyte k;
        char *zip;
        dbyte *rip1, *rip2, *rip3, *rip4;

        zip = sp_image;
        rip1 = (dbyte *) imag->data; /* TODO: fix warning */

        switch (scrmul) {
        case 1:
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        *rip1++ = (dbyte)
                            xsp_colors[(int) ((unsigned char) *zip++)];
                    }
                }
                else {
                    rip1 += TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 2:
            rip2 = rip1 + 2 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (dbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = k;
                        *rip1++ = *rip2++ = k;
                    }
                    rip1 += 2 * TV_WIDTH;  rip2 += 2 * TV_WIDTH;
                }
                else {
                    rip1 += 4 * TV_WIDTH;  rip2 += 4 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 3:
            rip2 = rip1 + 3 * TV_WIDTH;
            rip3 = rip2 + 3 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (dbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                    }
                    rip1 += 6 * TV_WIDTH;  rip2 += 6 * TV_WIDTH;
                    rip3 += 6 * TV_WIDTH;
                }
                else {
                    rip1 += 9 * TV_WIDTH;  rip2 += 9 * TV_WIDTH;
                    rip3 += 9 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 4:
            rip2 = rip1 + 4 * TV_WIDTH;
            rip3 = rip2 + 4 * TV_WIDTH;
            rip4 = rip3 + 4 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (dbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                    }
                    rip1 += 12 * TV_WIDTH;  rip2 += 12 * TV_WIDTH;
                    rip3 += 12 * TV_WIDTH;  rip4 += 12 * TV_WIDTH;
                }
                else {
                    rip1 += 16 * TV_WIDTH;  rip2 += 16 * TV_WIDTH;
                    rip3 += 16 * TV_WIDTH;  rip4 += 16 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;
        }
    }

    if(xsp_bpp == 32) {

        int i, j;
        qbyte k;
        char *zip;
        qbyte *rip1, *rip2, *rip3, *rip4;

        zip = sp_image;
        rip1 = (qbyte *) imag->data; /* TODO: fix warning */

        switch (scrmul) {
        case 1:
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        *rip1++ = (qbyte) 
                            xsp_colors[(int) ((unsigned char) *zip++)];
                    }
                }
                else {
                    rip1 += TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 2:
            rip2 = rip1 + 2 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (qbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = k;
                        *rip1++ = *rip2++ = k;
                    }
                    rip1 += 2 * TV_WIDTH;  rip2 += 2 * TV_WIDTH;
                }
                else {
                    rip1 += 4 * TV_WIDTH;  rip2 += 4 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 3:
            rip2 = rip1 + 3 * TV_WIDTH;
            rip3 = rip2 + 3 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (qbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                        *rip1++ = *rip2++ = *rip3++ = k;
                    }
                    rip1 += 6 * TV_WIDTH;  rip2 += 6 * TV_WIDTH;
                    rip3 += 6 * TV_WIDTH;
                }
                else {
                    rip1 += 9 * TV_WIDTH;  rip2 += 9 * TV_WIDTH;
                    rip3 += 9 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;

        case 4:
            rip2 = rip1 + 4 * TV_WIDTH;
            rip3 = rip2 + 4 * TV_WIDTH;
            rip4 = rip3 + 4 * TV_WIDTH;
            for(i = 0; i < TV_HEIGHT; i++) {
                if(sp_border_update ||
                   (i >= Y_OFF && i < TV_HEIGHT - Y_OFF &&
                    sp_imag_mark[i - Y_OFF])) {
                    if(i >= Y_OFF && i < TV_HEIGHT - Y_OFF)
                        sp_imag_mark[i - Y_OFF] = 0;
                    for(j = TV_WIDTH; j; j--) {
                        k = (qbyte) xsp_colors[(int) ((unsigned char) *zip++)];
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                        *rip1++ = *rip2++ = *rip3++ = *rip4++ = k;
                    }
                    rip1 += 12 * TV_WIDTH;  rip2 += 12 * TV_WIDTH;
                    rip3 += 12 * TV_WIDTH;  rip4 += 12 * TV_WIDTH;
                }
                else {
                    rip1 += 16 * TV_WIDTH;  rip2 += 16 * TV_WIDTH;
                    rip3 += 16 * TV_WIDTH;  rip4 += 16 * TV_WIDTH;
                    zip += TV_WIDTH;
                }
            }
            return;
        }
    }

    sprintf(msgbuf, "Translation not implemented (bpp: %i; scrmul: %i)",
            xsp_bpp, scrmul);
    put_msg(msgbuf);
    exit(1);
}


static void put_image(int x, int y, int w, int h, int toflush) 
{
#if 0
    static int serial = 0, num = 0;
    static int percent = 0;

    percent += (w * h) * 100 / (wwidth * wheight);
    if(toflush) {
        fprintf(stderr, "put_image/%i  %i  %i%%\n", 
                ++num, serial++, percent), num = 0, percent = 0;
        getchar();
    }

    else num++;
#endif

#ifdef HAVE_MITSHM
    if(is_shm_image) {
        XShmPutImage(xsp_disp, xsp_win, xsp_gc, imag, x, y, x, y, 
                     (unsigned) w, (unsigned) h, False);
        if(toflush) XSync(xsp_disp, False);
        return;
    }
#endif
    XPutImage(xsp_disp, xsp_win, xsp_gc, imag, x, y, x, y, 
              (unsigned) w, (unsigned) h);
    if(toflush) XFlush(xsp_disp);

}

static void get_min_max(unsigned val, int btnum, int *minp, int *maxp)
{
    int i, j, m;

    for(i = btnum, j = 0; i && !(val & 1); val >>= 1, j++, i--);
    *minp = j;
    for(m = 0; i; val >>=1, j++, i--) if(val & 1) m = j;
    *maxp = m;
}

void update_screen(void)
{
    if(!immed_image && (sp_border_update || sp_imag_horiz)) translate_image();
  
    if(sp_border_update && sp_imag_horiz) put_image(0, 0, wwidth, wheight, 1);
    else {
        if(exp_need) {
            int exmin, exmax, eymin, eymax;
      
            exmin = MAX(0, MIN(wwidth, exp_xmin));
            exmax = MAX(0, MIN(wwidth, exp_xmax));
      
            eymin = MAX(0, MIN(wheight, exp_ymin));
            eymax = MAX(0, MIN(wheight, exp_ymax));

          
            put_image(exmin, eymin, exmax - exmin, eymax - eymin, 
                      (!sp_border_update && !sp_imag_horiz));
        }    
        if(sp_border_update) {
            register int bhw, bvw, rh;
      
            bhw = X_OFF * scrmul;
            bvw = Y_OFF * scrmul;
            rh = HEIGHT * scrmul;
      
            put_image(0, 0, wwidth, bvw, 0);
            put_image(0, bvw, bhw, rh, 0);
            put_image((X_OFF+WIDTH) * scrmul, bvw, bhw, rh, 0);
            put_image(0, (Y_OFF+HEIGHT) * scrmul, wwidth, bvw, 1);
        }
        else if(sp_imag_horiz) {
            int xmin, xmax;
            int ymin, ymax;
      
            get_min_max(sp_imag_horiz, 32, &xmin, &xmax);
            get_min_max(sp_imag_vert, 24, &ymin, &ymax);
      
            put_image((xmin * 8 + X_OFF) * scrmul, 
                      (ymin * 8 + Y_OFF) * scrmul, 
                      (xmax - xmin + 1) * 8 * scrmul,
                      (ymax - ymin + 1) * 8 * scrmul, 1);
        }
    }

    exp_need = 0;
}

static void expose_call(XEvent *ev, void *ptr)
{
    int x1, x2, y1, y2;

    ptr = ptr;
    x1 = (*ev).xexpose.x;
    x2 = x1 + (*ev).xexpose.width;

    y1 = (*ev).xexpose.y;
    y2 = y1 +(*ev).xexpose.height;

    if(!exp_need) {
        exp_xmin = x1;
        exp_xmax = x2;
        exp_ymin = y1;
        exp_ymax = y2;
        exp_need = 1;
    }
    else {
        exp_xmin = MIN(exp_xmin, x1);
        exp_xmax = MAX(exp_xmax, x2);
        exp_ymin = MIN(exp_ymin, y1);
        exp_ymax = MAX(exp_ymax, y2);
    }

    if((*ev).xexpose.count == 0) {
        if(!xscr_delayed_expose) {
            int xmin, xmax, ymin, ymax;

            xmin = MAX(0, MIN(wwidth, exp_xmin));
            xmax = MAX(0, MIN(wwidth, exp_xmax));
      
            ymin = MAX(0, MIN(wheight, exp_ymin));
            ymax = MAX(0, MIN(wheight, exp_ymax));

            put_image(xmin, ymin, xmax - xmin, ymax - ymin, 1);
            exp_need = 0;
        }
    }
}


static void misc_event(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if(ev->type == ClientMessage) {
        /* If we get a client message which has the value of the delete
         * atom, it means the window manager wants us to die.
         */
        if ((*ev).xclient.data.l[0] == (unsigned char) delete_atom) exit(0);
    }
}


#ifdef HAVE_MITSHM

extern int      XShmQueryExtension (Display * dpy);
static int      haderror;
static int      (*origerrorhandler) (Display *, XErrorEvent *);

static int shmattacherrorhandler(Display *disp, XErrorEvent *err)
{
    if (err->error_code == BadAccess) haderror = 1;
    else (*origerrorhandler) (disp, err);

    return 0;
}

static char *create_shm_image(int width, int height, 
                              Visual *vis, unsigned dp, int bits)
{
    int scmsize;
    int status;
    void *res;

    scmsize = (bits * width * height) / 8;

    is_shm_image = 0;

    imag = XShmCreateImage(xsp_disp, vis, 
                           (unsigned) dp, ZPixmap, 0, &shminfo, 
                           (unsigned) width, (unsigned) height);

    if(imag == NULL) {
        put_msg("XShmCreateImage failed");
        return NULL;
    }

    shminfo.shmid = shmget(IPC_PRIVATE, (size_t) scmsize, IPC_CREAT | 0777);

    if(shminfo.shmid < 0) {
        sprintf(msgbuf, "Warning: shmget failed: %s", strerror(errno));
        put_msg(msgbuf);
        return NULL;
    }

    res = shmat(shminfo.shmid, 0, 0);
    shminfo.shmaddr = (char *) res;
  
    if((long) res == -1) {
        sprintf(msgbuf, "Warning: shmat failed: %s", strerror(errno));
        put_msg(msgbuf);
        return NULL;
    }
 
    imag->data = shminfo.shmaddr;

    shminfo.readOnly = False;

    haderror = 0;
    origerrorhandler = XSetErrorHandler(shmattacherrorhandler);
    status = XShmAttach(xsp_disp, &shminfo);
    XSync(xsp_disp, False);
    XSetErrorHandler (origerrorhandler);

    if(!status || haderror) {
        put_msg("XShmAttach failed");

        shmctl(shminfo.shmid, IPC_RMID, 0);
        shmdt(shminfo.shmaddr);
        shm_avail = 0;
        return NULL;
    }


    if(shmctl(shminfo.shmid, IPC_RMID, 0) < 0) {
        sprintf(msgbuf, "shmctl failed: %s", strerror(errno));
        put_msg(msgbuf);
        exit(1);
    }

    is_shm_image = 1;

    return shminfo.shmaddr;
}

static void destroy_shm_image(void)
{
    XDestroyImage(imag);

    XShmDetach(xsp_disp, &shminfo);
    shmdt(shminfo.shmaddr);
}


static void shm_query_extension(void)
{
    shm_avail = 1;
#ifdef HAVE_SHMQUERY
    if (!XShmQueryExtension(xsp_disp)) shm_avail = 0;
#endif
}

#endif /* HAVE_MITSHM */

static char *create_image(int width, int height,
                          Visual *vis, unsigned dp, int bits)
{
    int scmsize;
    char *idat;

    scmsize = (bits * width * height) / 8;

    idat = (char *) malloc_err((size_t) scmsize);

    imag = XCreateImage(xsp_disp, vis, 
                        (unsigned) dp, ZPixmap, 0, idat, 
                        (unsigned) width, (unsigned) height, 32, 0);

    return idat;
}

static void destroy_image(void)
{
    XDestroyImage(imag);
}

void spxs_init_color(void)
{
    if(xsp_bpp == 8) {
        int i;
        for(i = 0; i < 16; i++) sp_colors[i] = (unsigned char) xsp_colors[i];
    }
    else {
        int i;
        for(i = 0; i < 16; i++) sp_colors[i] = (unsigned char) i;
    }
    spscr_init_mask_color();
}

void init_spect_scr(void)
{
    idp = NULL;

#ifdef HAVE_MITSHM
    if(shm_avail && use_shm) 
        idp = create_shm_image(wwidth, wheight, xsp_visual, xsp_depth,
                               xsp_bpp);
#endif

    if(idp == NULL) {
        if(shm_first_check) {
            put_msg("Note: MITSHM extension not available.");
            shm_first_check = 0;
        }

        idp = create_image(wwidth, wheight, xsp_visual, xsp_depth, xsp_bpp);
    }
  
    if(scrmul != 1 || xsp_bpp != 8) {
        immed_image = 0;
        sp_image = (char *) malloc_err(TV_WIDTH * TV_HEIGHT);
    }
    else {
        immed_image = 1;
        sp_image = idp;
    }

    XMapWindow(xsp_disp, xsp_win);  
    XMapWindow(xsp_disp, top_win);  

    XSync(xsp_disp, False);

    spxs_init_color();
    spscr_init_line_pointers(TV_HEIGHT);
}

void destroy_spect_scr(void)
{
    XSync(xsp_disp, False);
  
    if(!immed_image) free(sp_image);

#ifdef HAVE_MITSHM
    if(is_shm_image) {
        destroy_shm_image();
        idp = NULL;
    }
#endif
  
    if(idp != NULL) {
        destroy_image();
        idp = NULL;
    }

    sp_image = NULL;
}


void resize_spect_scr(int s)
{
    if(s == scrmul || s > SCRMULMAX || s < 1) return;

    XResizeWindow(xsp_disp, top_win,
                  (unsigned) (TV_WIDTH * s), (unsigned) (TV_HEIGHT * s));
    clear_keystates();
}

static void resize_call(XEvent *ev, void *ptr)
{
    int w, h, x, y, s;

    ptr = ptr;
#if 0
    fprintf(stderr,"Resize Call: %d %d\n",
            (*ev).xconfigure.width, (*ev).xconfigure.height);
#endif

    w = (*ev).xconfigure.width;
    h = (*ev).xconfigure.height;

    if(w == wwidth && h == wheight) 
        return;

    x = (w + TV_WIDTH / 2) / TV_WIDTH;
    y = (h + TV_HEIGHT / 2) / TV_HEIGHT;
    s = MAX(x,y);

    if(s < 1) s = 1;
    else if(s > SCRMULMAX) s = SCRMULMAX;  
    if(s != scrmul) {
        destroy_spect_scr();
        scrmul = s;
    
        wwidth = TV_WIDTH * scrmul;
        wheight = TV_HEIGHT * scrmul;

        init_spect_scr();
    
        sp_init_screen_mark();           /* Redraw screen */
    }

    XResizeWindow(xsp_disp, xsp_win, (unsigned) wwidth, (unsigned) wheight);

    if(wwidth != w || wheight != h) 
        XResizeWindow(xsp_disp, top_win, (unsigned) wwidth,
                      (unsigned) wheight);
}

static void map_event(XEvent *ev, void *ptr)
{
    static int paused_bak = 0;
    ptr = ptr;

    if(ev->type == MapNotify) {
        if(pause_on_iconify && sp_paused) sp_paused = paused_bak;
        screen_visible = 1;
        accept_keys = 1;
    }
    else {
        if(pause_on_iconify) paused_bak = sp_paused, sp_paused = 1;

        if(wm_hints != NULL) {
            wm_hints->icon_pixmap = sp_paused ? pauseicon : runicon;
            wm_hints->flags = IconPixmapHint;
    
            XSetWMHints(xsp_disp, top_win, wm_hints);
        }


        clear_keystates();
        screen_visible = 0;
        accept_keys = 0;
    }
}

static void visibility_event(XEvent *ev, void *ptr)
{
    ptr = ptr;

    if((*ev).xvisibility.state == VisibilityFullyObscured) screen_visible = 0;
    else screen_visible = 1;
}


void init_x_scr(aX_default_resources* defres, int *argcp, char *argv[])
{
    XSetWindowAttributes attr;

    XSizeHints    *size_hints;
    XClassHint    *class_hints;
    XTextProperty window_name, *wnp;
    XTextProperty icon_name, *inp;

    unsigned long valuemask;
    XGCValues values;
    Atom proto_atom;

    shm_avail = 0;
    shm_first_check = 1;

    spscr_init_colors();

    xsp_disp = defres->disp;
    xsp_scr = defres->scr;
    xsp_visual = DefaultVisual(xsp_disp, xsp_scr);
    xsp_depth = DisplayPlanes(xsp_disp, xsp_scr);
    xsp_bpp = bits_per_pixel(xsp_depth);

#ifdef HAVE_MITSHM
    shm_query_extension();
#endif

    if(!shm_avail) {
        put_msg("Note: MITSHM extension not available");
        shm_first_check = 0;
    }

    if(!scrmul) {
        scrmul = 2;
        if(!shm_avail) scrmul = 1;

        while(scrmul > 1 &&
              (DisplayWidth(xsp_disp, xsp_scr) < TV_WIDTH * scrmul ||
               DisplayHeight(xsp_disp, xsp_scr) < TV_HEIGHT * scrmul))
            scrmul--;
    }

    wwidth = TV_WIDTH * scrmul;
    wheight = TV_HEIGHT * scrmul;
  
    top_win = XCreateSimpleWindow(xsp_disp, 
                                  RootWindow(xsp_disp, xsp_scr),
                                  0, 0,                     /* x, y */
                                  (unsigned) wwidth, 
                                  (unsigned) wheight,       /* width, height */
                                  0, 0,                     /* bw, bc */
                                  BlackPixel(xsp_disp, xsp_scr));

    xsp_win = XCreateSimpleWindow(xsp_disp, 
                                  top_win,
                                  0, 0,                     /* x, y */
                                  (unsigned) wwidth, 
                                  (unsigned) wheight,       /* width, height */
                                  0, 0,                     /* bw, bc */
                                  BlackPixel(xsp_disp, xsp_scr));


    if(!allocate_colors()) put_msg("Could not allocate colors");

    attr.colormap = xsp_cmap;
    attr.background_pixel = xsp_colors[7];
    attr.bit_gravity = ForgetGravity;
    XChangeWindowAttributes(xsp_disp, xsp_win,
                            CWBitGravity | CWBackPixel | CWColormap, 
                            &attr);



    runicon = XCreateBitmapFromData(xsp_disp, top_win, (char *) run_bits,
                                    run_width, run_height);

    pauseicon = XCreateBitmapFromData(xsp_disp, top_win, (char *) pause_bits,
                                      pause_width, pause_height);

    maskicon = XCreateBitmapFromData(xsp_disp, top_win, (char *) mask_bits,
                                     mask_width, mask_height);


  
    size_hints = XAllocSizeHints();
    if(size_hints != NULL) {
        size_hints->min_width = TV_WIDTH;
        size_hints->min_height = TV_HEIGHT;
        size_hints->max_width = SCRMULMAX * TV_WIDTH;
        size_hints->max_height = SCRMULMAX * TV_HEIGHT;
        size_hints->width_inc = TV_WIDTH;
        size_hints->height_inc = TV_HEIGHT;
        size_hints->min_aspect.x = TV_WIDTH;
        size_hints->min_aspect.y = TV_HEIGHT;
        size_hints->max_aspect.x = TV_WIDTH;
        size_hints->max_aspect.y = TV_HEIGHT;
        size_hints->base_width = 0;
        size_hints->base_height = 0;
#if 1
        size_hints->flags = PMinSize | PMaxSize | PResizeInc | PAspect |
            PBaseSize;
#else
        size_hints->flags = 0;
#endif
    }

    wm_hints = XAllocWMHints();
    if(wm_hints != NULL) {
        wm_hints->input = True;
        wm_hints->initial_state = NormalState;
        wm_hints->icon_pixmap = runicon;
        wm_hints->icon_mask = maskicon;
        wm_hints->flags = InputHint | StateHint | IconPixmapHint |
            IconMaskHint;
    }

    class_hints = XAllocClassHint();
    if(class_hints != NULL) {
        class_hints->res_name  = (char *) defres->prog_name; 
        class_hints->res_class = (char *) defres->class_name;
        /* TODO: fix warning */
    }
  
    wnp = &window_name;
    if(!XStringListToTextProperty(&defres->window_name, 1, wnp)) wnp = NULL;
  
    inp = &icon_name;
    if(!XStringListToTextProperty(&defres->icon_name, 1, inp)) inp = NULL;

    XSetWMProperties(xsp_disp, top_win, wnp, inp, argv, *argcp, 
                     size_hints, wm_hints, class_hints);
  
    if(size_hints != NULL)  XFree((void *) size_hints);
    if(class_hints != NULL) XFree((void *) class_hints);

    /* Delete-Window-Message black magic copied from xloadimage. */
    proto_atom  = XInternAtom(xsp_disp,"WM_PROTOCOLS", False);
    delete_atom = XInternAtom(xsp_disp,"WM_DELETE_WINDOW", False);
    if ((proto_atom != None) && (delete_atom != None)) {
        XChangeProperty(xsp_disp, top_win, proto_atom, XA_ATOM, 32,
                        PropModePrepend, (unsigned char*)&delete_atom, 1);
    }

    valuemask = GCForeground;
    values.foreground = xsp_colors[0];

    xsp_gc = XCreateGC(xsp_disp, xsp_win, valuemask, &values);


    aX_add_event_proc(xsp_disp, xsp_win, Expose,
                      expose_call, ExposureMask, NULL);

    aX_add_event_proc(xsp_disp, top_win, ConfigureNotify,
                      resize_call, StructureNotifyMask, NULL);

    aX_add_event_proc(xsp_disp, top_win, FocusIn, 
                      xkey_focus_change_call, FocusChangeMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, FocusOut, 
                      xkey_focus_change_call, FocusChangeMask, NULL);

    aX_add_event_proc(xsp_disp, top_win, ClientMessage,
                      misc_event, 0, NULL);
    aX_add_event_proc(xsp_disp, top_win, CirculateNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, CreateNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, GravityNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, ReparentNotify,
                      misc_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, top_win, DestroyNotify,
                      misc_event, StructureNotifyMask, NULL);

    aX_add_event_proc(xsp_disp, xsp_win, MapNotify,
                      map_event, StructureNotifyMask, NULL);
    aX_add_event_proc(xsp_disp, xsp_win, UnmapNotify,
                      map_event, StructureNotifyMask, NULL);
  
    aX_add_event_proc(xsp_disp, xsp_win, VisibilityNotify, 
                      visibility_event, VisibilityChangeMask, NULL);


    init_xutils();
}
