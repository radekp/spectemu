/* 
 * Other parts of this program are (C) 1996-1998 Szeredi Miklos
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
 * This part of program has been written by
 * Mikko Nummelin (mnummeli@cc.hut.fi) in 2001-11-20
 * and is now just a beta version.
 * File spkey.c has been slightly modified.
 * New files are memdmps.c and memdmps.h
 * The makefile has of course, been modified to support this.
 */

#include <stdio.h>
#include <string.h>
#include "memdmps.h"
#include "spperif.h"

int memdmp_write(unsigned char *startpt_p, int length, char *filename_p){
  
    FILE *ofile_p;
  
    ofile_p = fopen(filename_p,"wb");
    if(ofile_p == NULL){
        return MEMDMP_FAILED;
    }
    fwrite(startpt_p,1,length,ofile_p);
    fclose(ofile_p);
    return MEMDMP_OK;
}

int memdmp_read(unsigned char *startpt_p, int length, char *filename_p){

    FILE *ifile_p;

    ifile_p = fopen(filename_p,"rb");
    if(ifile_p == NULL){
        return MEMDMP_FAILED;
    }
    fread(startpt_p,1,length,ifile_p);
    fclose(ifile_p);
    sp_init_screen_mark();
    return MEMDMP_OK;
}

int memdmp_lst(unsigned char *startpt_p, int length, int startpt){

    int i,j;

    printf("    :  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    printf("----:------------------------------------------------\n");
    for(i=0;i<(length/16);i++){
        printf("%3X : ",((startpt+(i<<4))>>4) & 0xFFF);
        for(j=0;j<16;j++){
            printf("%2X ",(startpt_p[16*i+j] & 0xFF));
        }
        printf("\n");
    }

    return MEMDMP_OK;
}

int memdmp_poke(unsigned char *startpt_p, char *hexvalues){

    int i,len,value;

    len = strlen(hexvalues)/2;
    if(len>16){
        len=16;
    }
    for(i=0;i<len;i++){
        value = char2_hexvalue(&(hexvalues[i*2]));
        startpt_p[i]=value;
    }
    sp_init_screen_mark();
    return MEMDMP_OK;
}

int char_hexvalue(char hn){
    if((hn>='0')&&(hn<='9'))
        return(hn-'0');
    if((hn>='a')&&(hn<='f'))
        return(hn-'a'+10);
    if((hn>='A')&&(hn<='F'))
        return(hn-'A'+10);
    return 0;
}

int char2_hexvalue(char *hn_2_p){
    return(16 * char_hexvalue(hn_2_p[0])+
           char_hexvalue(hn_2_p[1]));
}

int char4_hexvalue(char *hn_4_p){
    return(4096 * char_hexvalue(hn_4_p[0])+
           256  * char_hexvalue(hn_4_p[1])+
           16   * char_hexvalue(hn_4_p[2])+
           char_hexvalue(hn_4_p[3]));
}

int memdmp_editor(unsigned char *mem_p){

    int startpt,length;
    char *nextarg;
    char command1[128];
    char firstcmd[16];
    char startptstr[16];
    char lengthstr[16];
    char contstr[33];
    char filename[128];
    char *place_p;
  
    printf("\nMiscellaneous functions (currently memory dump"
           " loading and saving).\n");
    printf("WARNING - THIS PART OF PROGRAM IS BETA VERSION!\n");
    printf("\nType:\n\n");
    printf("i [start in 4-digit hex] [amount of bytes in 4-digit hex]\n");
    printf("for dumping mem contents on screen.\n\n");
    printf("o [start in 4-digit hex] [bytes in 2-digit hex each]\n");
    printf("for POKE [address],[byte1], POKE [address],[byte2] etc.\n\n");
    printf("l [start in 4-digit hex] [amount of bytes in 4-digit hex]");
    printf(" [filename]\nfor loading.\n\n");
    printf("s [start in 4-digit hex] [amount of bytes in 4-digit hex]");
    printf(" [filename]\nfor saving.\n\n");
    printf("q to quit.\n\n");

    fgets(command1,127,stdin);
    nextarg=strtok(command1," ");
    strncpy(firstcmd,nextarg,15);

    if((firstcmd[0]=='o') ||
       (firstcmd[0]=='O')){
        nextarg = strtok(NULL," ");
        strncpy(startptstr,nextarg,15);
        nextarg = strtok(NULL," ");
        strncpy(contstr,nextarg,33);
        startpt = char4_hexvalue(startptstr);
        memdmp_poke(&(mem_p[startpt]),contstr);

        printf("Memory successfully POKEd.\n");
    }

    if((firstcmd[0]=='l') ||
       (firstcmd[0]=='s') ||
       (firstcmd[0]=='i') ||
       (firstcmd[0]=='L') ||
       (firstcmd[0]=='S') ||
       (firstcmd[0]=='I')){
    
        nextarg = strtok(NULL," ");
        strncpy(startptstr,nextarg,15);
        nextarg = strtok(NULL," ");
        strncpy(lengthstr,nextarg,15);
        nextarg = strtok(NULL," ");

        if(nextarg!=NULL){
            strncpy(filename,nextarg,127);
        } else {
            strncpy(filename,"DUMMY.MEM",127);
        }

        startpt = char4_hexvalue(startptstr);
        length  = char4_hexvalue(lengthstr);

        for(place_p=filename;place_p<=(filename+127);place_p++){
            if(*place_p==10){
                *place_p=0;
                break;
            }
        }

        if((firstcmd[0]=='l')||(firstcmd[0]=='L')){
            memdmp_read(&mem_p[startpt],length,filename);
            printf("Memory successfully read.\n");
        } else if((firstcmd[0]=='s')||(firstcmd[0]=='S')){
            memdmp_write(&mem_p[startpt],length,filename);
            printf("Memory successfully written.\n");
        } else if((firstcmd[0]=='i')||(firstcmd[0]=='I')){
            memdmp_lst(&mem_p[(startpt&0xFFF0)],length,startpt);
            printf("Memory contents successfully listed.\n");
        }
    }

    printf("Exiting Miscellaneous functions menu.\n");
    return MEMDMP_OK;
}
