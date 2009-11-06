/* 
 * Copyright (C) 1999 Matan Ziv-Av
 * Email: zivav@cs.bgu.ac.il
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
 * less sensitive joystick readings patched by
 * Matthias Arndt <marndt@asmsoftware.de>
 * on July 29 2002.
 *
 */

#include "config.h"

#ifdef LINUX_JOYSTICK
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/joystick.h>
#endif

#include "spjoy.h"

int joystick_status = 0;

#ifdef LINUX_JOYSTICK
static int linux_joystick_fd;
static struct js_event linux_joystick_js;

#define JOY_THRESHOLD 16383

void joystick_open()
{
    linux_joystick_fd = open("/dev/js0", O_RDONLY | O_NONBLOCK);
}

void joystick_update() {

    if(linux_joystick_fd == -1) {
        joystick_status = 0;
        return;
    }

    if((read(linux_joystick_fd, &linux_joystick_js, sizeof(struct js_event))) ==
       sizeof(struct js_event)) {
        switch(linux_joystick_js.type & ~JS_EVENT_INIT) {
        case JS_EVENT_BUTTON:
            if(linux_joystick_js.number==0) {
                if(linux_joystick_js.value==0) joystick_status &= 0xf;
                else joystick_status |= 0x10;
            }
            break;
        case JS_EVENT_AXIS:
            switch(linux_joystick_js.number) {
            case 0:
                if((linux_joystick_js.value > -JOY_THRESHOLD) &&
                   (linux_joystick_js.value < JOY_THRESHOLD))
                    joystick_status &= 0x1c;
                if(linux_joystick_js.value <= -JOY_THRESHOLD)
                    joystick_status = (joystick_status & 0x1c) | 0x02;
                if(linux_joystick_js.value >= JOY_THRESHOLD)
                    joystick_status = (joystick_status & 0x1c) | 0x01;
                break;
            case 1:
                if((linux_joystick_js.value > -JOY_THRESHOLD) &&
                   (linux_joystick_js.value < JOY_THRESHOLD))
                    joystick_status &= 0x13;
                if(linux_joystick_js.value <= -JOY_THRESHOLD)
                    joystick_status = (joystick_status & 0x13) | 0x08;
                if(linux_joystick_js.value >= JOY_THRESHOLD)
                    joystick_status = (joystick_status & 0x13) | 0x04;
                break;
            }
            break;
        }
    }
}

#else /* No Joystick */

void joystick_open()
{
}

void joystick_update() 
{
}

#endif
