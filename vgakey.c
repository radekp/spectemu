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

#include "spkey.h"
#include "spkey_p.h"
#include "spperif.h"

#include "vgascr.h"

#include "akey.h"

#include <stdlib.h>

const int need_switch_mode = 1;
volatile int spvk_after_switch = 0;

#define KC_F1 59

#define KC_L_SHIFT 42
#define KC_R_SHIFT 54

#define KC_L_CTRL 29
#define KC_R_CTRL 97

#define KC_L_ALT 56
#define KC_R_ALT 100

#ifndef DOS

#include <vgakeyboard.h>

static char *kbstate;

static void spkb_init(void)
{
    keyboard_init();
    keyboard_translatekeys(DONT_CATCH_CTRLC);
    kbstate = keyboard_getstate();
}

#define spkb_close()   keyboard_close()
#define spkb_raw()     keyboard_init()
#define spkb_normal()  keyboard_close()
#define spkb_update()  keyboard_update()

#define kbst(i) kbstate[i]

#else /* !DOS */

#include <kb.h>

#define spkb_init()    kb_install(0)
#define spkb_close()   kb_remove()
#define spkb_raw()     kb_install(0)
#define spkb_normal()  kb_remove()
#define spkb_update()  kb_update()

#define kbst(i) kb_key(i)

#endif /* !DOS */

#define SDEMPTY {0, 0}
#define SDSPEC(x) {x, x}

struct spscan_keydef {
    unsigned norm;
    unsigned shifted;
};

static struct spscan_keydef sp_cp_keycode[] = {
    SDEMPTY,
    SDSPEC(SK_Escape),
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    SDSPEC(SK_BackSpace),
   
    SDSPEC(SK_Tab),
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
    {'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    SDSPEC(SK_Return),
   
    SDSPEC(SK_Control_L),
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '"'}, 
    {'`', '~'},
   
    SDSPEC(SK_Shift_L),
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    SDSPEC(SK_Shift_R),
    SDSPEC(SK_KP_Multiply),
    SDSPEC(SK_Alt_L),
   
    {' ', ' '},
    SDSPEC(SK_Caps_Lock),
    SDSPEC(SK_F1),
    SDSPEC(SK_F2),
    SDSPEC(SK_F3),
    SDSPEC(SK_F4),
    SDSPEC(SK_F5),
    SDSPEC(SK_F6),
    SDSPEC(SK_F7),
    SDSPEC(SK_F8),
    SDSPEC(SK_F9),
    SDSPEC(SK_F10),

    SDSPEC(SK_Num_Lock),
    SDSPEC(SK_Scroll_Lock),
    SDSPEC(SK_KP_Home),
    SDSPEC(SK_KP_Up),
    SDSPEC(SK_KP_Page_Up),
    SDSPEC(SK_KP_Subtract),
    SDSPEC(SK_KP_Left),
    SDSPEC(SK_KP_5),
    SDSPEC(SK_KP_Right),
    SDSPEC(SK_KP_Add),
    SDSPEC(SK_KP_End),
    SDSPEC(SK_KP_Down),
    SDSPEC(SK_KP_Page_Down),
    SDSPEC(SK_KP_Insert),
    SDSPEC(SK_KP_Delete),

    SDEMPTY,
    SDEMPTY,
    {'<', '>'},
    SDSPEC(SK_F11),
    SDSPEC(SK_F12),
    SDEMPTY,
    SDEMPTY,
    SDEMPTY,
    SDEMPTY,
    SDEMPTY,
    SDEMPTY,
    SDEMPTY,
    SDSPEC(SK_KP_Enter),
    SDSPEC(SK_Control_R),
    SDSPEC(SK_KP_Divide),
    SDSPEC(SK_Print),
    SDSPEC(SK_Alt_R),
    SDEMPTY,
    SDSPEC(SK_Home),
    SDSPEC(SK_Up),
    SDSPEC(SK_Page_Up),
    SDSPEC(SK_Left),
    SDSPEC(SK_Right),
    SDSPEC(SK_End),
    SDSPEC(SK_Down),
    SDSPEC(SK_Page_Down),
    SDSPEC(SK_Insert),
    SDSPEC(SK_Delete),
};

#define LASTKEYCODE 111

spkeyboard kb_mkey;

void spkey_textmode(void)
{
    spkb_normal();
    restore_sptextmode();
}

void spkey_screenmode(void)
{
    set_vga_spmode();
    spkb_raw();
  
    sp_init_screen_mark();
}


void spkb_process_events(int evenframe)
{
    int i;
    int changed;
  
    if(evenframe) {

        spkb_update();

        changed = 0;
        for(i = 0; i <= LASTKEYCODE; i++) {
            int ki;
            unsigned sh;
            unsigned ks;
      
            ks = sp_cp_keycode[i].norm;
            ki = KS_TO_KEY(ks);
      
            if(kbst(i) && !spkb_kbstate[ki].press) { /* Press */
                changed = 1;
                spkb_kbstate[ki].press = 1;
        
                sh = sp_cp_keycode[i].shifted;
        
                /* TODO: Do something with CapsLock, NumLock */
                spkb_last.modif = 0;

                if(kbst(KC_L_SHIFT) || kbst(KC_R_SHIFT)) 
                    spkb_last.modif |= SKShiftMask;

                if(kbst(KC_L_CTRL) || kbst(KC_R_CTRL)) 
                    spkb_last.modif |= SKControlMask;

                if(kbst(KC_L_ALT) || kbst(KC_R_ALT)) 
                    spkb_last.modif |= SKMod1Mask;
        
                spkb_last.index = ki;
                spkb_last.shifted = sh;
        
                spkb_last.keysym = (spkb_last.modif &  SKShiftMask) ? sh : ks;
        
                if((!ISFKEY(ks) || !spvk_after_switch) && accept_keys &&
                   !spkey_keyfuncs()) {
                    spkb_kbstate[ki].state = 1;
                    spkb_kbstate[i].frame = sp_int_ctr;
                }
                else spkb_kbstate[ki].state = 0;
        
                spvk_after_switch = 0;
            }
      
            if(!kbst(i) && spkb_kbstate[ki].press) { /* Release */
                changed = 1;
                spkb_kbstate[ki].press = 0;
                spkb_kbstate[ki].state = 0;
            }
        }
        if(changed) spkb_state_changed = 1;
    }
    
    process_keys();
}

static void close_spect_key(void)
{
    spkb_close();
}

void init_spect_key(void)
{
    int i;

    for(i = 0; i < NR_SPKEYS; i++) spkb_kbstate[i].press = 0;
    clear_keystates();
    init_basekeys();

    spkb_init();
  
    atexit(close_spect_key);
}

int display_keyboard(void)
{
    return 0;
}
