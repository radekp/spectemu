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

#include "misc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define DIR_SEP_CHAR '/'

char *get_base_name(char *fname)
{
    char *p;
  
    p = fname;
    for(; *p; p++);
    for(; p >= fname && *p != DIR_SEP_CHAR; p--);
    return ++p;
}


int check_ext(const char *filename, const char *ext)
{
    int flen, elen;
    int i;

    flen = (int) strlen(filename);
    elen = (int) strlen(ext);

    if(flen <= elen + 1) return 0;
  
    if(filename[flen-elen-1] != '.') return 0;
    for(i = 0; i < elen; i++) if(filename[flen-elen+i] != toupper(ext[i])) break;
    if(i == elen) return 1;
    for(i = 0; i < elen; i++) if(filename[flen-elen+i] != tolower(ext[i])) break;
    if(i == elen) return 1;
    return 0;
}

void add_extension(char *filename, const char *ext)
{
    int i;
    int upper;

    upper = 1;
    for(i = 0; filename[i]; i++) if(islower((int) filename[i])) upper = 0;
  
    filename[i++] = '.';
    if(upper)
        for(; *ext; i++, ext++) filename[i] = toupper(*ext);
    else 
        for(; *ext; i++, ext++) filename[i] = tolower(*ext);
}

int file_exist(const char *filename)
{
    FILE *fp;

    fp = fopen(filename, "rb");
    if(fp != NULL) {
        fclose(fp);
        return 1;
    }
    if(errno == ENOENT) return 0;
    return 1;
}

int try_extension(char *filename, const char *ext)
{
    int tend;
  
    tend = (int) strlen(filename);
    add_extension(filename, ext);
    if(file_exist(filename)) return 1;

    filename[tend] = '\0';
    return 0;
}

void *malloc_err(size_t size)
{
    char *p;

    p = (char *) malloc(size);
    if(p == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    return (void *) p;
}

char *make_string(char *ostr, const char *nstr)
{
    if(ostr != NULL) free(ostr);
    ostr = malloc_err(strlen(nstr) + 1);
    strcpy(ostr, nstr);
    return ostr;
}

void free_string(char *ostr)
{
    if(ostr != NULL) free(ostr);
}

int mis_strcasecmp(const char *s1, const char *s2)
{
    int c1, c2;
  
    for(;; s1++, s2++) {
        c1 = tolower(*s1);
        c2 = tolower(*s2);

        if(!c1 || c1 != c2) break;
    }
    return c1-c2;
}
