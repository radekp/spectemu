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

#include "spmain.h"
#include "spperif.h"
#include "misc.h"
#include "xscr.h"
#include "spconf.h"

#include "ax.h"

int main(int argc, char *argv[])
{
    aX_default_resources defres;
    spma_init_privileged();
    spma_lose_privs();
  
    spcf_pre_check_options(argc, argv);
  
    defres.width = 0;
    defres.height = 0;
    defres.x = 0;
    defres.y = 0;
    defres.border_width = 10;
    defres.foreground = 0;
    defres.background = 1;
    defres.font_name = "7x14";
    defres.fallback_font_name = NULL;
    defres.prog_name = get_base_name(argv[0]);
    defres.class_name = "XSpscreen";

    aX_open_disp(NULL, 0, &argc, argv, &defres);

    check_params(argc, argv);
    sp_init();

    init_x_scr(&defres, &argc, argv);
  
    start_spectemu();
    /* This function DOES NOT return */

    return 0;
}
