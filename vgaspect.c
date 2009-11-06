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
#include "vgascr.h"
#include "spconf.h"

int main(int argc, char *argv[])
{
    spma_init_privileged();
    sp_init_vga();

    spcf_pre_check_options(argc, argv);

    check_params(argc, argv);
    sp_init();

    start_spectemu();
    /* This function DOES NOT return */

    return 0;
}


