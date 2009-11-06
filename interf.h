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

#ifndef INTERF_H
#define INTERF_H

#define MAXFILENAME 1024

extern char filenamebuf[];
extern char msgbuf[];
extern int spif_can_print;

extern char *spif_get_filename(void);
extern char *spif_get_tape_fileinfo(int *startp, int *nump);
extern void put_msg(const char *msg);
extern void put_tmp_msg(const char *msg);

#endif /* INTERF_H */
