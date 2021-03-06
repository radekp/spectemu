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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define FIELDS_PER_LINE 10
#define EXTENSION ".c"

static char *progname;

static void bin_to_c(FILE* ifp, FILE *ofp, char *name, 
                     char *iname, char *oname)
{
    int i;
    int c;
    int first;
    unsigned long ctr;

    fprintf(ofp, "/* %s */\n\n", oname);

    fprintf(ofp, 
            "/*\n"
            "   This file was generated by %s from binary image\n"
            "   file `%s'\n"
            " */\n\n", 
            progname, iname);

    fprintf(ofp, "unsigned char %s[] = {", name);

    i = 0;
    first = 1;
    ctr = 0;
    while((c = getc(ifp)) != EOF) {
        ctr++;

        if(!first) fprintf(ofp, ", ");
        else first = 0;

        if(!i) fprintf(ofp, "\n  ");

        fprintf(ofp, "0x%02X", (int) ((unsigned char) c));
    
        i = (i + 1) % FIELDS_PER_LINE;
    }
  
    fprintf(ofp, "\n};\n\n");
    fprintf(ofp, "const unsigned long %s_size = %lu;\n\n", name, ctr);
    fprintf(ofp, "/* End of %s */\n", oname);
}


int main(int argc, char *argv[])
{
    char *inputfile, *outputfile, *outprefix;
    char *ext;
    FILE *ifp, *ofp;

    progname = argv[0];

    if(argc != 2) {
        fprintf(stderr, "usage: %s inputfile\n", progname);
        return 1;
    }

    inputfile = argv[1];

    outputfile = malloc(strlen(inputfile) + strlen(EXTENSION) + 1);
    if(outputfile == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        return 1;
    }
  
    strcpy(outputfile, inputfile);
    ext = strrchr(outputfile, '.');
    if(ext != 0)
        *ext = '\0';

    outprefix = malloc(strlen(outputfile) + 1);
    if(outprefix == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        return 1;
    }
    strcpy(outprefix, outputfile);
    strcat(outputfile, EXTENSION);
  
    ifp = fopen(inputfile, "rb");
    if(ifp == NULL) {
        fprintf(stderr, "Could not open input file `%s': %s\n", 
                inputfile, strerror(errno));
        return 1;
    }
  
    ofp = fopen(outputfile, "wt");
    if(ofp == NULL) {
        fprintf(stderr, "Could not open output file `%s': %s\n", 
                outputfile, strerror(errno));
        return 1;
    }

    bin_to_c(ifp, ofp, outprefix, inputfile, outputfile);

    free(outputfile);
    free(outprefix);
  
    fclose(ofp);
    fclose(ifp);

    return 0;
}
