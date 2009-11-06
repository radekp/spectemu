/*
 * SPCONV -	ZX-Spectrum 48K snapshot converter
 *
 *		Public Domain software
 *
 * Author: Henk de Groot
 *         SNX conversion routines added by Damien Burke (v1.09)
 */

#if defined(__STDC__) || defined(__TURBOC__)
#include <stdlib.h>
#else
#include <sys/types.h>
#endif

#ifdef __TURBOC__
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#ifdef __TURBOC__
#include <alloc.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include "spconv.h"

struct snx_s snx;
struct sna_s sna;
struct vga_s vga;
struct z80_s z80;
struct prg_s prg;
struct ach_s ach;
struct kgb_s kgb;

/* extra space just in case the Z80 decompress fails due to a corrupt image */
unsigned char image[IMSIZE+0x100];
unsigned int z80_size;

signed int	intype;
signed int	outtype;

/* Prototype for main */
extern int main(int argc, char *argv[]);

/* Path names are limited to 80 characters under MSDOS so this sould be */
/* enough... If you are using UNIX than you may have to make this array */
/* larger.                                                              */
char	my_directory[120];

int main(int argc, char *argv[])
{
	char		*p;
	struct stat	status;
	const  char	*fromstring;
	const  char	*tostring;
	char		*outfile;
	long		file_size;

#ifdef __TURBOC__
	/* for Trubo-C: open all files in binary mode */
	extern int _fmode;
	_fmode = O_BINARY;
#endif

	if(argc != 3)
		USAG_usage();

	if((strchr(argv[1],'*')!=NULL) || (strrchr(argv[1],'?')!=NULL) ||
	   (strchr(argv[2],'*')!=NULL) || (strrchr(argv[2],'?')!=NULL))	{
		fprintf(stderr,"This program can't handle wildcards, sorry!\n");
		return EXIT_USAGE;
	}

	/*
	 * Find the directory path where this program is started from.
	 * This will be needed for finding the spectrum-rom. Some formats
	 * need the spectrum-rom for proper conversion.
	 */
	strcpy(my_directory,argv[0]);
	if(strrchr(my_directory,'\\')!=NULL) {
		*strrchr(my_directory,'\\')='\0';
	}
	else if(strrchr(my_directory,'/')!=NULL) {
		*strrchr(my_directory,'/')='\0';
	}
	else if(strrchr(my_directory,':')!=NULL) {
		*strrchr(my_directory,':')='\0';
	} else {
		my_directory[0]='\\';
		my_directory[1]='\0';
	}

	/*
	 * Check if the input file exists and fetch the file lenght of
	 * the file in the mean time. Two for the prise of one...
	 */
	if(stat(argv[1],&status)<0) {
		perror(argv[1]);
		return EXIT_FILE_ERROR;
	}
	file_size = status.st_size;

	/*
	 * recognize input type on filename:
	 *
	 *  	.SNX    ->	SNX file (used by Atari emulator)
	 *	.SNA	->	SNA file (used in JPP)
	 *	.SP	->	SP file (was VGASPEC)
	 *	.Z80	->	Z80 file
	 *	.PRG	->	PRG file
	 *	.ACH	->	ACH file (from archimedes emulator)
	 *	.ZX	->	KGB file (from KGB emulator)
	 *	other	->	if exact 48+header -> raw file
	 *	otherwise 	unknown
	 */

	fromstring = DTYP_determine_type(argv[1],&intype);

	if (intype == UNKNOWN) {
		if (file_size == (hdr_size+IMSIZE)) {
			fromstring="RAW";
			intype=RAW;
		} else {
			fprintf(stderr,"Unknown input file format. Must be a valid .SNX, .SNA, .SP, .Z80, .PRG, .ACH or\n");
			fprintf(stderr,".ZX file, or a Raw file\n");
			return EXIT_FILE_ERROR;
		}
	}

	/*
	 * recognize output type on filename:
	 *
	 *  	.SNX	->	SNX file
	 *	.SNA	->	SNA file
	 *	.SP	->	SP file (was VGASPEC)
	 *	.Z80	->	Z80 file
	 *	.PRG	->	PRG file
	 *	.ACH	->	ACH file (from archimedes emulator)
	 *	.ZX	->	KGB file (from KGB emulator)
	 *	otherwise 	unknown
	 */

	tostring = DTYP_determine_type(argv[2],&outtype);

	if(outtype==UNKNOWN) {
		fprintf(stderr,"Unknown output file format. Must be a .SNX, .SNA, .SP, .Z80, .PRG, .ACH or .ZX file\n");
		return EXIT_FILE_ERROR;
	}

	/*
	 * if argv[2] only contains the suffix then use the prefix of
	 * argv[1];
	 */
	if(argv[2][0]=='.') {
		outfile=malloc(strlen(argv[1])+strlen(argv[2])+1);
		strcpy(outfile,argv[1]); /* copy prefix    */

		p=strrchr(outfile,'.');
		if(p!=NULL) *p='\0'; /* put end of string at position of '.' */

		strcat(outfile,argv[2]); /* append suffix  */
	} else {
		outfile=malloc(strlen(argv[2])+1);

		strcpy(outfile,argv[2]);
	}

	/*
	 * Special converion between versions of the same type
	 */
	if(intype == outtype)
		return DIRC_direct_convert(intype, argv[1], outfile);

	/*
	 * General conversion from one type to another
	 */

	printf("Converting %s from %s to %s\n",argv[1],fromstring,tostring);

	/*
	 * convert input_file to SNA
	 */

	switch(intype) {
	case SNX:	RSNX_read_snx(argv[1]);
			SXSN_snx_to_sna();
			break;
	case SNA:	if (file_size == (sna_size+IMSIZE)) {
				RSNA_read_sna(argv[1]);
			}
			break;
	case SP:	if ((file_size == (vga_size+IMSIZE))) {
				RVGA_read_vgaspec(argv[1]);
				VGSN_vgaspec_to_sna();
			} else if ((file_size == (vga_size+IMSIZE-6))) {
				ROVG_read_old_vgaspec(argv[1]);
				VGSN_vgaspec_to_sna();
			}
			break;
	case RAW:	RRAW_read_raw(argv[1]);
			RASN_raw_to_sna();
			break;
	case Z80:	RZ80_read_z80(argv[1]);
			Z8SN_z80_to_sna();
			break;
	case PRG:	if(file_size != (prg_size+IMSIZE)) {
				printf("Warning: the image part of %s is not exactly 48k!\n",argv[1]);
				printf("         Converting anyway, the converted file may not work\n");
			}
			RPRG_read_prg(argv[1]);
			PRSN_prg_to_sna();
			break;
	case ACH:	if(file_size == (ach_size+16384L+IMSIZE)) {
				RACH_read_ach(argv[1]);
				ACSN_ach_to_sna();
			}
			break;
	case KGB:	if(file_size == (132L+IMSIZE+kgb_size)) {
				RKGB_read_kgb(argv[1]);
				KGSN_kgb_to_sna();
			}
			break;
	default:	printf("Unrecognized input file type, can't convert\n");
			return EXIT_FILE_ERROR;
	}

	/*
	 * convert internal SNA format to output file
	 */

	switch(outtype) {
	case SNX:       SNSX_sna_to_snx();
			WSNX_write_snx(outfile);
			break;
	case SNA:	WSNA_write_sna(outfile);
			break;
	case SP:	SNVG_sna_to_vgaspec();
			WVGA_write_vgaspec(outfile);
			break;
	case Z80:	SNZ8_sna_to_z80();
			WZ80_write_z80(outfile);
			break;
	case PRG:	SNPR_sna_to_prg(outfile);
			WPRG_write_prg(outfile);
			break;
	case ACH:	SNAC_sna_to_ach();
			WACH_write_ach(outfile);
			break;
	case KGB:	SNKG_sna_to_kgb();
			WKGB_write_kgb(outfile);
			break;
	default:	printf("Unrecognized output file type, can't convert\n");
			return EXIT_FILE_ERROR;
	}

	/*
	 * That's it, simple isn't it?
	 */
	return EXIT_OK;
}

/* SPECIAL FUNCTIONS */

/* Determine file type and return both type and type-name */

const char * DTYP_determine_type(char * filename, signed int * type)
{
	const		char *p;
	char		ext[5];
	signed int	i;

	/*
	 * recognize type on filename:
	 *
	 *	.SNX	->	SNX file
	 *	.SNA	->	SNA file
	 *	.SP	->	SP file (was VGASPEC)
	 *	.Z80	->	Z80 file
	 *	.PRG	->	PRG file
	 *	.ACH	->	ACH file (from archimedes emulator)
	 *	.ZX	->	KGB file (from KGB emulator)
	 *	otherwise 	unknown
	 */

	*type=UNKNOWN;	/* default if all ifs below fail */

	p=strrchr(filename,'.');
	if(p==NULL) p=filename; /* not found, set at begin of string */

	/* Take extension and convert to uppercase */
	for(i = 0; i < 4; i++) {
		if(*p != '\0') {
			ext[i] = toupper(*p);
			p++;
		} else {
			ext[i] = '\0';
		}
	}
	ext[4] = '\0'; /* termination at least on the fifth char */

	if(strcmp(ext,".SNX")==0) {
		*type=SNX;
		return ".SNX";
	}
	if(strcmp(ext,".SNA")==0) {
		*type=SNA;
		return ".SNA";
	}
	if(strcmp(ext,".Z80")==0) {
		*type=Z80;
		return ".Z80";
	}
	if(strcmp(ext,".SP")==0) {
		*type=SP;
		return ".SP";
	}
	if(strcmp(ext,".PRG")==0) {
		*type=PRG;
		return ".PRG";
	}
	if(strcmp(ext,".ACH")==0) {
		*type=ACH;
		return ".ACH";
	}
	if(strcmp(ext,".ZX")==0) {
		*type=KGB;
		return ".ZX";
	}

	/* unknown type, return question-mark */
	return "?";
}

/* Special converion between versions of the same type */

signed int DIRC_direct_convert(signed int type, char * infile, char * outfile)
{
	struct stat status;
	signed long file_size;

	/*
	 * Special conversion between versions of the same type.
	 * These conversion don't use the intermediate 'sna' format
	 * to preserve as much information as possible.
	 *
	 * This currently only applies to .SP and the .Z80 formats.
	 *
	 * The .SP formats will convert from the New to the Old format or
	 * from the Old to the New format, controled by the current format
	 * of the input.
	 *
	 * The .Z80 formats will convert to the version 1.45 .Z80 format
	 * since version 1.45 .Z80 files can be read by both old and new
	 * Z80-emulators.
	 */

	/* check if it is for the SP format */
	if(type == SP) {
		if(stat(infile,&status)<0) {
			perror(infile);
			return EXIT_FILE_ERROR;
		}
		file_size = status.st_size;

		if((file_size == (vga_size+IMSIZE))) {
			printf("Converting %s from new .SP format to old .SP format.\n",infile);
			RVGA_read_vgaspec(infile);

			WOVG_write_old_vgaspec(outfile);
			return EXIT_OK;
		}

		if((file_size == (vga_size+IMSIZE-6))) {
			RVGH_read_vgaspec_header(infile);

			if((vga.S=='S')&&(vga.P=='P')) {
fprintf(stderr,"Invalid input file format. This could be a new syle .SP file with\n");
fprintf(stderr,"an image of another length than 48Kb. This kind of .SP files cannot\n");
fprintf(stderr,"be converted. All other file formats (including the old .SP format)\n");
fprintf(stderr,"contain images of 48Kb length.\n");
				return EXIT_FILE_ERROR;
			}

			printf("Converting %s from old .SP format to new .SP format.\n",infile);
			ROVG_read_old_vgaspec(infile);

			/* These values are fixed for a full 48K .SP file */
			vga.S='S';
			vga.P='P';
			vga.len_l=0x00;
			vga.len_h=0xC0;
			vga.start_l=0x00;
			vga.start_h=0x40;

			WVGA_write_vgaspec(outfile);

			return EXIT_OK;
		} else {
			RVGH_read_vgaspec_header(infile);
			if((vga.S=='S')&&(vga.P=='P')) {
fprintf(stderr,"Invalid input file format. This could be a new syle .SP file with\n");
fprintf(stderr,"an image of another length than 48Kb. This kind of .SP files cannot\n");
fprintf(stderr,"be converted. All other file formats (including the old .SP format)\n");
fprintf(stderr,"contain images of 48Kb length.\n");
			} else {
fprintf(stderr,"Unknown input file format. Must be a valid .SNA, .SP, .Z80, .PRG, .ACH, .ZX or RAW file\n");
			}
			return EXIT_FILE_ERROR;
		}
	}

	if(type == Z80)
	{
		printf("Converting %s from .Z80 to .Z80 (output in version 1.45 format)\n",infile);
		RZ80_read_z80(infile);

		WZ80_write_z80(outfile);
		return EXIT_OK;
	}

	/* If we get here than we had no special handling for this type */
	/* return an error in that case...                              */
	fprintf(stderr,"Input and output file format are the same. ");
	fprintf(stderr,"What you try to do\n");
	fprintf(stderr,"is handled much better by the MSDOS \"COPY\" ");
	fprintf(stderr,"command!\n");

	return EXIT_USAGE;
}

/* ERROR HANDLING FUNCTIONS */

/* Print Usage and exit */
void USAG_usage()
{
	fprintf(stderr,"SPCONV version 1.09 - %s\n\n",__DATE__);
	fprintf(stderr,"Usage: spconv <source> <target>\n\n");
	fprintf(stderr,"Source must be a valid .SNA, .SP, .Z80, .PRG, .ACH, .ZX, .SNX or RAW file.\n");
	fprintf(stderr,"Target must be a .SNA, .SP, .Z80, .PRG, .ACH, .ZX or .SNX file.\n\n");
	fprintf(stderr,"If the second parameter contains only a suffix, the prefix\n");
	fprintf(stderr,"of the input file will be used (i.e. 'spconv file.sna .z80')\n\n");
	fprintf(stderr,"Output .SP files are in the new format, .Z80 files are compressed\n");
	fprintf(stderr,"in the version 1.45 .Z80 format\n\n");
	fprintf(stderr,"If <source> and <target> are .SP files, convertion from old\n");
	fprintf(stderr,"to new format or from new to old format will be performed.\n");
	fprintf(stderr,"If <source> and <target> are .Z80 files, convertion to\n");
	fprintf(stderr,"the version 1.45 format will be performed.\n");
	fprintf(stderr,"If <source> and <target> are of the same type an error message\n");
	fprintf(stderr,"will be generated (unless they are both .SP or .Z80 files)\n");
	fprintf(stderr,"\n\nPublic Domain, H. de Groot & D. Burke 1995\n\n");

	exit(EXIT_USAGE);
}

void RERR_read_error(char * s, FILE * fd)
{
	perror(s);
	if(fd != NULL) fclose(fd);
	exit(EXIT_READ_ERROR);
}

void WERR_write_error(char * s, FILE * fd)
{
	perror(s);
	if(fd != NULL) fclose(fd);
	exit(EXIT_WRITE_ERROR);
}

/* I/O FUNCTIONS */

/* GENERIC I/O - READ/WRITE SNAPSHOT */

void RGEN_read_generic(char * s, void * header, size_t h_size)
{
	FILE * fd;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	if(fread(header, h_size, 1, fd)!=1)
		RERR_read_error(s, fd);

	if(fread(image, (size_t) IMSIZE, 1, fd)!=1)
		RERR_read_error(s, fd);

	fclose(fd);
}

void WGEN_write_generic(char * s, void * header, size_t h_size)
{
	FILE * fd;

	unlink(s);

	fd=fopen(s,"w");
	if(fd == NULL)
		WERR_write_error(s, fd);

	if(fwrite(header, h_size, 1, fd)!=1)
		WERR_write_error(s, fd);

	if(fwrite(image, (size_t) IMSIZE, 1, fd)!=1)
		WERR_write_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE .SNA IMAGE */

void RSNA_read_sna(char * s)
{
	RGEN_read_generic(s, (void *) &sna, sna_size);
}

void WSNA_write_sna(char * s)
{
	WGEN_write_generic(s, (void *) &sna, sna_size);
}

/* SPECIFIC I/O - READ NEW .SP HEADER */

void RVGH_read_vgaspec_header(char * s)
{
	FILE * fd;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	if(fread(&vga,vga_size,1,fd)!=1)
		RERR_read_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE NEW .SP IMAGE */

void RVGA_read_vgaspec(char * s)
{
	RGEN_read_generic(s, (void *) &vga, vga_size);
}

void WVGA_write_vgaspec(char * s)
{
	WGEN_write_generic(s, (void *) &vga, vga_size);
}

/* SPECIFIC I/O - READ/WRITE OLD .SP IMAGE */

void ROVG_read_old_vgaspec(char * s)
{
	RGEN_read_generic(s, (void *)(((char *)&vga)+6), (unsigned int) (vga_size-6));
}

void WOVG_write_old_vgaspec(char * s)
{
	WGEN_write_generic(s, (void *)(((char *)&vga)+6), (unsigned int) (vga_size-6));
}

/* SPECIFIC I/O - READ RAW IMAGE */

void RRAW_read_raw(char * s)
{
	signed int i;
	FILE       * fd;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	if(fread(&h, hdr_size, 1, fd)!=1)
		RERR_read_error(s, fd);

	/* check if the image was saved the correct way */
	for(i=0;i<9;i++) {
		if(h.in[i]!=expect[i]) {
			fprintf(stderr,"Header of Spectrum image not ok, ");
			fprintf(stderr,"Spectrum image should be saved with:\n");
			fprintf(stderr,"SAVE *\"b\"CODE 16384,49152");
			fclose(fd);
			exit(EXIT_FILE_ERROR);
		}
	}

	if(fread(image, (size_t) IMSIZE, 1, fd)!=1)
		RERR_read_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE .Z80 IMAGE */

void RZ80_read_z80(char * s)
{
	FILE       * fd;
	signed int ext_size;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	/* read old header part */
	if(fread(&z80,z80_145_size,1,fd)!=1)
		RERR_read_error(s, fd);

	/* check for 2.01 format */
	if((z80.pch == (unsigned char) 0) &&
	   (z80.pcl == (unsigned char) 0)) {

		/* 2.01 or better format, check if we can use this */
		unsigned char *p;
		p = (unsigned char *) &z80;

		/* read an aditional 2 bytes */
		if(fread(&p[z80_145_size],2,1,fd)!=1)
			RERR_read_error(s, fd);

		ext_size = ((signed int) p[z80_145_size]) +
				 256 * ((signed int) p[z80_145_size+1]);

		if(ext_size < z80_201_ext_size) {
			fprintf(stderr,"%s seems to be a new type Z80 file!\n",s);
			fprintf(stderr,"This program is only tested for files up to version 3.0 format.\n");
			fprintf(stderr,"The .Z80 file doesn't seem to be upwards compatible, since the\n");
			fprintf(stderr,"extended header seems to be smaller than the version 2.01 extended\n");
			fprintf(stderr,"header. The program can NOT convert this file!\n");
			fclose(fd);
			exit(EXIT_FILE_ERROR);
		}
		if(
			 (ext_size != z80_201_ext_size)
			 &&
			 (ext_size != z80_300_ext_size )
		  ) {
			printf("Warning: %s seems to be a new type Z80 file!\n",s);
			printf("         This program is only tested for files up to version 3.0 format.\n");
			printf("         Assuming upwards compatibiliy, the program will attempt to convert\n");
			printf("         the file anyway. The resulting file may not work!.\n");
		}

		/* we passed this test, read the first part of the extended header */
		if(fread(&p[z80_145_size+2], z80_201_ext_size, 1, fd) != 1)
			RERR_read_error(s, fd);

		/* seek over the remaining extended header part if needed */
		if(ext_size > z80_201_ext_size)
		{
			if (fseek(fd, (long) (ext_size - z80_201_ext_size), SEEK_CUR) != 0)
				RERR_read_error(s, fd);
		}

		/* we got a complete header now. */
		/* check the type of the file... */
		switch (ext_size) {
		case z80_201_ext_size:
			if((unsigned int) z80.hardware >= 3) {
				fprintf(stderr,"%s is not a 48K Spectrum Z80 file, can't convert!\n",s);
				fclose(fd);
				exit(EXIT_FILE_ERROR);
			}
			break;
		case z80_300_ext_size:
			if((unsigned int) z80.hardware >= 4) {
				fprintf(stderr,"%s is not a 48K Spectrum Z80 file, can't convert!\n",s);
				fclose(fd);
				exit(EXIT_FILE_ERROR);
			}
			break;
		default:
			if((unsigned int) z80.hardware >= 4) {
				printf("Warning: %s might not be a 48K Spectrum Z80 file!\n",s);
				printf("         (hardware flag in .Z80 file >= 4)\n");
				printf("         Assuming 48K Spectrum anyway, continuing...\n");
			}
			break;
		}

		/* check if the interface-1 rom was paged in */
		if(z80.if1_paged != (unsigned char) 0) {
			fprintf(stderr,"%s has interface-1 rom paged in, can't convert!\n",s);
			fclose(fd);
			exit(EXIT_FILE_ERROR);
		}
		/* all fine up till now, put PC back to the old place and reset the */
		/* compressed memory bit, the z80_read_page function decompresses.  */
		z80.pch = z80.n_pch;
		z80.pcl = z80.n_pcl;
		z80.data = z80.data & ~0x20; /* reset compressed mode bit to be sure */

		while(RDPG_z80_read_page(s, fd)!=0);
	} else {
		/* The file is in version 1.45 format.             */
		/* Read and decompress if the image is compressed, */
		/* just read if it is not compressed at all...     */

		if((z80.data & 0x20)!=0) {
			Z80D_z80_decompress(fd,0,(unsigned int) IMSIZE);
			z80.data = z80.data & ~0x20; /* reset compressed mode bit to be sure */
		} else {
			if(fread(image, (size_t) IMSIZE, 1, fd)!=1)
				RERR_read_error(s, fd);
		}
	}
	fclose(fd);
}

void WZ80_write_z80(char * s)
{
	FILE * fd;

	/* Try to compress the data */
	z80.data=z80.data | Z80C_z80_compress();

	unlink(s);

	fd=fopen(s,"w");
	if(fd == NULL)
		WERR_write_error(s, fd);

	if(fwrite(&z80, z80_145_size, 1, fd)!=1)
		WERR_write_error(s, fd);

	if(fwrite(image, (size_t) z80_size, 1, fd)!=1)
		WERR_write_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE .PRG IMAGE */

void RPRG_read_prg(char * s)
{
	RGEN_read_generic(s, (void *) &prg, prg_size);
}

void WPRG_write_prg(char * s)
{
	WGEN_write_generic(s, (void *) &prg, prg_size);
}

/* SPECIFIC I/O - READ/WRITE .ACH IMAGE */

void RACH_read_ach(char * s)
{
	FILE * fd;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	if(fread(&ach,ach_size,1,fd)!=1)
		RERR_read_error(s, fd);

	/* fseek over the 16K ram area */
	if(fseek(fd,16384L,SEEK_CUR)!=0)
		RERR_read_error(s, fd);

	if(fread(image, (size_t) IMSIZE, 1, fd)!=1)
		RERR_read_error(s, fd);

	fclose(fd);
}

void WACH_write_ach(char * s)
{
	char		  buffer[1024];
	char		  * p;
	signed int        i;
	struct stat       status;
	const char	  * rom;
	FILE              * fd;
	signed long       file_size;

	/* clean buffer first */
	p=(char *) buffer;
	for(i=0; i < 1024; i++) p[i]='\0';

	unlink(s);

	fd=fopen(s,"w");
	if(fd == NULL)
		WERR_write_error(s, fd);

	if(fwrite(&ach,ach_size,1,fd)!=1)
		WERR_write_error(s, fd);

	strcat(my_directory,"\\spectrum.rom");

	rom=NULL;

	if(stat("spectrum.rom",&status)>=0) {
		rom="spectrum.rom";
	}
	else if (stat(my_directory,&status)>=0)	{
		rom=(const char *) my_directory;
	}
	file_size = status.st_size;

	if(rom==NULL) {
		printf("Warning: The file \"spectrum.rom\" needed for proper conversion to the .ach\n");
		printf("         format could not be located, Converting anyway. The ROM space\n");
		printf("         will be filled with 0 bytes, the converted file may not work\n");

		/* write the 16K ram area as zero's */
		for(i=0; i < 16; i++) {
			if(fwrite(buffer,1024,1,fd)!=1)
				WERR_write_error(s, fd);
		}
	} else {
		if (file_size != 16384) {
		printf("Warning: The file \"spectrum.rom\" needed for proper conversion to the .ach\n");
		printf("         format has a wrong size (not 16K). Converting anyway. The ROM space\n");
		printf("         will be filled with 0 bytes, the converted file may not work\n");

			/* copy the 16K ROM area */
			for(i=0; i < 16; i++) {
				if(fwrite(buffer,1024,1,fd)!=1)
					WERR_write_error(s, fd);
			}
		} else {
			FILE * fd_specrom;

			printf("Using Spectrum ROM: %s\n",rom);

			fd_specrom=fopen(rom,"r");
			if(fd_specrom==NULL) {
				perror(rom);
				fclose(fd);
				exit(EXIT_WRITE_ERROR);
			}

			for(i=0; i < 16; i++) {
				if(fread(buffer,1024,1,fd_specrom)!=1) {
					perror(rom);
					fclose(fd_specrom);
					fclose(fd);
					exit(EXIT_WRITE_ERROR);
				}
				if(fwrite(buffer,1024,1,fd)!=1) {
					perror(s);
					fclose(fd_specrom);
					fclose(fd);
					exit(EXIT_WRITE_ERROR);
				}
			}
			fclose(fd_specrom);
		}
	}

	if(fwrite(image, (size_t) IMSIZE, 1, fd)!=1)
		WERR_write_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE .ZX IMAGE */

void RKGB_read_kgb(char * s)
{
	FILE * fd;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	/* fseek over the first 132 bytes */
	if(fseek(fd,132L,SEEK_CUR)!=0)
		RERR_read_error(s, fd);

	if(fread(image, (size_t) IMSIZE, 1, fd)!=1)
		RERR_read_error(s, fd);

	if(fread(&kgb,kgb_size,1,fd)!=1)
		RERR_read_error(s,fd);

	fclose(fd);
}

void WKGB_write_kgb(char * s)
{
	char		  buffer[132];
	char		  * p;
	signed int        i;
	struct stat       status;
	const char        * rom;
	FILE              * fd;
	signed long       file_size;

	/* clean buffer first */
	p=(char *) buffer;
	for(i=0; i < 132; i++) p[i]='\0';

	unlink(s);

	fd=fopen(s,"w");
	if(fd == NULL)
		WERR_write_error(s, fd);

	strcat(my_directory,"\\spectrum.rom");

	rom=NULL;

	if(stat("spectrum.rom",&status)>=0) {
		rom="spectrum.rom";
	}
	else if (stat(my_directory,&status)>=0) {
		rom=(const char *) my_directory;
	}
	file_size = status.st_size;

	if(rom==NULL) {
		printf("Warning: The file \"spectrum.rom\" needed for proper conversion to the .zx\n");
		printf("         format could not be located, Converting anyway. The 132 bytes needed\n");
		printf("         from the ROM will be filled with 0 bytes, the converted file may not\n");
		printf("         work\n");

		/* write the 132 byte area as zero's */
		if(fwrite(buffer,132,1,fd)!=1)
			WERR_write_error(s, fd);
	} else {
		if (file_size != 16384) {
		printf("Warning: The file \"spectrum.rom\" needed for proper conversion to the .zx\n");
		printf("         format has a wrong size (not 16K). Converting anyway. The 132 bytes\n");
		printf("         needed from the ROM will be filled with 0 bytes, the converted file\n");
		printf("         may not work\n");

			/* write the 132 byte area as zero's */
			if(fwrite(buffer,132,1,fd)!=1)
				WERR_write_error(s, fd);
		} else {
			FILE * fd_specrom;

			printf("Using Spectrum ROM: %s\n",rom);

			fd_specrom=fopen(rom,"r");

			if(fd_specrom == NULL) {
				perror(rom);
				fclose(fd);
				exit(EXIT_WRITE_ERROR);
			}

			/* fseek over the first 16384-132 bytes */
			if(fseek(fd_specrom,16252L,SEEK_CUR)!=0) {
				perror(rom);
				fclose(fd_specrom);
				fclose(fd);
				exit(EXIT_WRITE_ERROR);
			}
			if(fread(buffer,132,1,fd_specrom)!=1) {
				perror(rom);
				fclose(fd_specrom);
				fclose(fd);
				exit(EXIT_WRITE_ERROR);
			}
			if(fwrite(buffer,132,1,fd)!=1) {
				perror(s);
				fclose(fd_specrom);
				fclose(fd);
				exit(EXIT_WRITE_ERROR);
			}
			fclose(fd_specrom);
		}
	}

	if(fwrite(image, (size_t) IMSIZE, 1, fd)!=1)
		WERR_write_error(s, fd);

	if(fwrite(&kgb,kgb_size,1,fd)!=1)
		WERR_write_error(s, fd);

	fclose(fd);
}

/* SPECIFIC I/O - READ/WRITE .SNX IMAGE */

void RSNX_read_snx(char * s)
{
	FILE       * fd;
	unsigned char	counthi, countlo, marker, fill;
	unsigned int	count, i, addr;

	fd=fopen(s,"r");
	if(fd == NULL)
		RERR_read_error(s, fd);

	/* read header part */
	if(fread(&snx,snx_size,1,fd)!=1)
		RERR_read_error(s, fd);

	if((snx.signature[0] != 'X') || (snx.signature[1] != 'S') ||
	   (snx.signature[2] != 'N') || (snx.signature[3] != 'A'))
	{
		printf("This is not a .SNX file, it should have the 'XSNA' text at the start!\n");
		RERR_read_error(s,fd);
	}

	/* If the header is an unexpected size, seek past it, ignoring any
	   extra information held in it */
	if (snx.headerlenhi * 256 + snx.headerlenlo != snx_size-6)
	{
		printf("Warning: the header part of %s is not the expected %u bytes!\n",s,snx_size);
		printf("         Converting anyway, the converted file may not work\n");
		fseek(fd,snx.headerlenhi*256+snx.headerlenlo+6,SEEK_SET);
	}
	/* Read and decompress the image. */
	addr = 0;
	while (addr < IMSIZE)
	{
		if(fread(&counthi,1,1,fd)!=1)
			RERR_read_error(s,fd);
		if (counthi >= 0xE0)
		{
			count = (counthi & 0x0F) + 1;
			if ((counthi & 0xF0) == 0xF0)
				marker = SNX_COMPRESSED;
			else
				marker = SNX_UNCOMPRESSED;
		}
		else
		{
			if(fread(&countlo,1,1,fd)!=1)
				RERR_read_error(s,fd);
			count = (counthi << 8) + countlo;
			if(fread(&marker,1,1,fd)!=1)
				RERR_read_error(s,fd);
		}

		if (addr + count > IMSIZE)
			RERR_read_error(s,fd);
		if (marker == SNX_COMPRESSED)
		{
			if(fread(&fill,1,1,fd)!=1)
				RERR_read_error(s,fd);
			for(i = 0; i < count; i++)
				image[addr + i] = fill;
			addr += count;
		}
		else
		{
			i = 0;
			while (i < count)
			{
				if(fread(&image[addr + i++],1,1,fd)!=1)
					RERR_read_error(s,fd);
			}
			addr += count;
		}
	}
	fclose(fd);
}

void WSNX_write_snx(char * s)
{
	FILE            * fd;
	unsigned char	marker, countlo, counthi;
	unsigned int	addr, i, j, count, packable;

	unlink(s);

	fd=fopen(s,"w");
	if(fd == NULL)
		WERR_write_error(s, fd);

	/* Write header first */
	if(fwrite(&snx,snx_size,1,fd)!=1)
		WERR_write_error(s, fd);

	/* Now compress and write RAM image */

	addr = 0;
	while (addr < IMSIZE)
	{
		i = addr;
		j = i;
		/* First find a run we can compress; must be at least 6
		   identical bytes in a row. */
		while ( (j+5 < IMSIZE) && (image[j] == image[j+1]) &&
					  (image[j] == image[j+2]) &&
					  (image[j] == image[j+3]) &&
					  (image[j] == image[j+4]) &&
					  (image[j] == image[j+5]) )
		{
			j++;
		}
		if (j > i)
		{
			count = (j - i) + 5;
			if (count <= 16)
			{
				counthi = 0xF0 + (count - 1);
				if(fwrite(&counthi,1,1,fd)!=1)
					WERR_write_error(s,fd);
			}
			else
			{
				counthi = count >> 8;
				countlo = count & 0xFF;
				if(fwrite(&counthi,1,1,fd)!=1)
					WERR_write_error(s,fd);
				if(fwrite(&countlo,1,1,fd)!=1)
					WERR_write_error(s,fd);
				marker = SNX_COMPRESSED;
				if(fwrite(&marker,1,1,fd)!=1)
					WERR_write_error(s,fd);
			}

			if(fwrite(&image[i],1,1,fd)!=1)
				WERR_write_error(s,fd);

			addr += count;
			i = j + 5;
		}

		j = i + 1;
		packable = count = 0;
		/* Now find a run of non-identical bytes */
		do
		{
			if (image[j] != image[j-1])
			{
				packable = 0;
				count++;
				j++;
			}
			else /* Found two consecutive identical bytes */
			{
				if ( (j+3 < IMSIZE) &&
				     (image[j] == image[j+1]) &&
				     (image[j] == image[j+2]) &&
				     (image[j] == image[j+3]) &&
				     (image[j] == image[j+4]) )
				{ /* Found six consecutive identical bytes */
					packable = 6;
					j--;
				}
				else
				{
					packable = 0;
					count++;
					j++;
				}
			}
		} while ( (j < IMSIZE) && (packable < 6) );

		if (count > 0)
		{
			if (IMSIZE - (i + count) < 6)
				count = (unsigned int)(IMSIZE - i);
			if (count <= 16)
			{
				counthi = 0xE0 + (count - 1);
				if(fwrite(&counthi,1,1,fd)!=1)
					WERR_write_error(s,fd);
			}
			else
			{
				counthi = count >> 8;
				countlo = count & 0xFF;
				if(fwrite(&counthi,1,1,fd)!=1)
					WERR_write_error(s,fd);
				if(fwrite(&countlo,1,1,fd)!=1)
					WERR_write_error(s,fd);
				marker = SNX_UNCOMPRESSED;
				if(fwrite(&marker,1,1,fd)!=1)
					WERR_write_error(s,fd);
			}

			while (i < j)
			{
				if(fwrite(&image[i++],1,1,fd)!=1)
					WERR_write_error(s,fd);
			}
			addr += count;
		}
	}

	fclose(fd);
}

/* CONVERSION FUNCTIONS - TO .SNA FORMAT */

void VGSN_vgaspec_to_sna()
{
	unsigned int addr;
	unsigned int sp;

	sna.f=vga.f;
	sna.a=vga.a;
	sna.b=vga.b;
	sna.c=vga.c;
	sna.d=vga.d;
	sna.e=vga.e;
	sna.h=vga.h;
	sna.l=vga.l;

	sna.fax=vga.fax;
	sna.aax=vga.aax;
	sna.bax=vga.bax;
	sna.cax=vga.cax;
	sna.dax=vga.dax;
	sna.eax=vga.eax;
	sna.hax=vga.hax;
	sna.lax=vga.lax;

	sna.ixh=vga.ixh;
	sna.ixl=vga.ixl;
	sna.iyh=vga.iyh;
	sna.iyl=vga.iyl;

	sna.border=vga.border;

	sna.i=vga.i;
	sna.r=vga.r;

	/* If register I has changed, chances are good that it runs in
	   IM2 mode */
	if(sna.i==0x3f)
		sna.im=0x01;
	else
		sna.im=0x02;

	if((vga.im & 0x01) == 0)
		sna.iff2=0x00;
	else
		sna.iff2=0xff;

	sp=256*vga.sph+vga.spl;
	sp=sp-2;
	addr=sp-0x4000;
	image[addr]=vga.pcl;
	image[addr+1]=vga.pch;

	sna.sph=sp/256;
	sna.spl=sp%256;
}

void RASN_raw_to_sna()
{
	unsigned int addr;
	unsigned int sp;
	unsigned int pc;

	pc=0x1bf4; /* entry of "next statement" */

	sna.f=0x99;
	sna.a=0x5f;
	sna.b=0x1f;
	sna.c=0xf0;
	sna.d=0x5d;
	sna.e=0x0c;
	sna.h=0x5d;
	sna.l=0x0e;

	sna.fax=0x44;
	sna.aax=0x00;
	sna.bax=0x18;
	sna.cax=0x20;
	sna.dax=0x00;
	sna.eax=0x07;
	sna.hax=0x5c;
	sna.lax=0xf1;

	sna.ixh=0x03;
	sna.ixl=0xd4;
	sna.iyh=0x5c;
	sna.iyl=0x3a;

	sna.i=0x3f;
	sna.r=0x00;
	sna.im=0x01;
	sna.iff2=0xFF;

	/* set sp by means of RAMTOP in the image */
	addr=0x5cb2-0x4000;
	sp=256*image[addr+1]+image[addr]-1;

	/* Reset ERR NR to no error */
	image[0x5c3a-0x4000]=0xff;
	
	/* Set border by means of BORDCR */
	sna.border=(image[0x5c48-0x4000] & 0x38)>>3;
	
	/* put return address to MAIN-4 (0x1303) on stack */
	sp=sp-2;
	addr=sp-0x4000;	
	image[addr]=0x03; 
	image[addr+1]=0x13;
	
	sp=sp-2;
	addr=sp-0x4000;
	image[addr]=pc%256;
	image[addr+1]=pc/256;

	sna.sph=sp/256;
	sna.spl=sp%256;
}

void Z8SN_z80_to_sna()
{
	unsigned int addr;
	unsigned int sp;

	sna.f=z80.f;
	sna.a=z80.a;
	sna.b=z80.b;
	sna.c=z80.c;
	sna.d=z80.d;
	sna.e=z80.e;
	sna.h=z80.h;
	sna.l=z80.l;

	sna.fax=z80.fax;
	sna.aax=z80.aax;
	sna.bax=z80.bax;
	sna.cax=z80.cax;
	sna.dax=z80.dax;
	sna.eax=z80.eax;
	sna.hax=z80.hax;
	sna.lax=z80.lax;

	sna.ixh=z80.ixh;
	sna.ixl=z80.ixl;
	sna.iyh=z80.iyh;
	sna.iyl=z80.iyl;

	sna.border=(z80.data/2) & 0x07; 

	sna.i=z80.i;

	if(z80.data==0xff) z80.data=0;

	if((z80.data & 0x01)==1)
		sna.r=(z80.r & 0x7f)+0x80;
	else
		sna.r=z80.r & 0x7f;

	sna.im=z80.im & 0x03;
	
	if(z80.iff2 != 0)
		sna.iff2=0xff;
	else
		sna.iff2=0x00;

	sp=256*z80.sph+z80.spl;
	sp=sp-2;
	addr=sp-0x4000;
		
	sna.sph=sp/256;
	sna.spl=sp%256;

	image[addr]=z80.pcl;
	image[addr+1]=z80.pch;
}

void PRSN_prg_to_sna()
{
	unsigned int addr;
	unsigned int sp;

	sna.b=prg.b;
	sna.c=prg.c;
	sna.d=prg.d;
	sna.e=prg.e;
	sna.h=prg.h;
	sna.l=prg.l;
	sna.fax=prg.fax;
	sna.aax=prg.aax;
	sna.bax=prg.bax;
	sna.cax=prg.cax;
	sna.dax=prg.dax;
	sna.eax=prg.eax;
	sna.hax=prg.hax;
	sna.lax=prg.lax;

	sna.ixh=prg.ixh;
	sna.ixl=prg.ixl;
	sna.iyh=prg.iyh;
	sna.iyl=prg.iyl;

	/* Set border by means of BORDCR */
	sna.border=(image[0x5c48-0x4000] & 0x38)>>3;

	sna.i=prg.i;

	if(prg.i==0x3f)
		sna.im=0x01;
	else
		sna.im=0x02;

	sp=256*prg.sph+prg.spl;
	sp=sp+4; /* there are two more words on the stack besides PC */
	addr=sp-0x4000;

	sna.r=image[addr-3];
	/* the af combo is on the stack */
	sna.f=image[addr-2];
	sna.a=image[addr-1];
		
	sna.sph=sp/256;
	sna.spl=sp%256;

	/* interrupts always on ??? */
	sna.iff2=prg.iff2;
}

void ACSN_ach_to_sna()
{
	unsigned int addr;
	unsigned int sp;

	sna.f=ach.f;
	sna.a=ach.a;
	sna.b=ach.b;
	sna.c=ach.c;
	sna.d=ach.d;
	sna.e=ach.e;
	sna.h=ach.h;
	sna.l=ach.l;

	sna.fax=ach.fax;
	sna.aax=ach.aax;
	sna.bax=ach.bax;
	sna.cax=ach.cax;
	sna.dax=ach.dax;
	sna.eax=ach.eax;
	sna.hax=ach.hax;
	sna.lax=ach.lax;

	sna.ixh=ach.ixh;
	sna.ixl=ach.ixl;
	sna.iyh=ach.iyh;
	sna.iyl=ach.iyl;

	sna.border=ach.border;

	sna.i=ach.i;

	sna.r=ach.r;

	sna.im=ach.im & 0x03;
	if(sna.im == 3)
		 sna.im = 0;
		
	sna.iff2=ach.iff2;

	sp=256*ach.sph+ach.spl;
	sp=sp-2;
	addr=sp-0x4000;
		
	sna.sph=sp/256;
	sna.spl=sp%256;

	image[addr]=ach.pcl;
	image[addr+1]=ach.pch;
}

void KGSN_kgb_to_sna()
{
	unsigned int addr;
	unsigned int sp;

	sna.f=kgb.f;
	sna.a=kgb.a;
	sna.b=kgb.b;
	sna.c=kgb.c;
	sna.d=kgb.d;
	sna.e=kgb.e;
	sna.h=kgb.h;
	sna.l=kgb.l;

	sna.fax=kgb.fax;
	sna.aax=kgb.aax;
	sna.bax=kgb.bax;
	sna.cax=kgb.cax;
	sna.dax=kgb.dax;
	sna.eax=kgb.eax;
	sna.hax=kgb.hax;
	sna.lax=kgb.lax;

	sna.ixh=kgb.ixh;
	sna.ixl=kgb.ixl;
	sna.iyh=kgb.iyh;
	sna.iyl=kgb.iyl;

	sna.i=kgb.i;
	sna.r=kgb.r;

	/* border-colour not found in KGB image */
	/* Set border by means of BORDCR */
	sna.border=(image[0x5c48-0x4000] & 0x38)>>3;

	/* determine interrupt mode using the value of register I */
	if (kgb.i_mode_l==0xff)
		sna.im=0x00;
	else if(kgb.i_mode_l==1)
		sna.im=0x02; 
	else
		sna.im=0x01;
		
	if((kgb.interruptstatus & 0x01) != 0)
		sna.iff2=0xff;
	else
		sna.iff2=0x0;

	sp=256*kgb.sph+kgb.spl;
	sp=sp-2;
	addr=sp-0x4000;
		
	sna.sph=sp/256;
	sna.spl=sp%256;

	image[addr]=kgb.pcl;
	image[addr+1]=kgb.pch;
}

/* CONVERSION FUNCTIONS - FROM .SNA FORMAT */

void SNVG_sna_to_vgaspec()
{
	unsigned int addr;
	unsigned int sp;
	unsigned int pc;

	sp=256*sna.sph+sna.spl;
	addr=sp-0x4000;
	pc=image[addr]+256*image[addr+1];
	sp=sp+2;

	vga.S='S';
	vga.P='P';
	vga.len_l=0x00;
	vga.len_h=0xC0;
	vga.start_l=0x00;
	vga.start_h=0x40;
	vga.f=sna.f;
	vga.a=sna.a;
	vga.b=sna.b;
	vga.c=sna.c;
	vga.d=sna.d;
	vga.e=sna.e;
	vga.h=sna.h;
	vga.l=sna.l;

	vga.fax=sna.fax;
	vga.aax=sna.aax;
	vga.bax=sna.bax;
	vga.cax=sna.cax;
	vga.dax=sna.dax;
	vga.eax=sna.eax;
	vga.hax=sna.hax;
	vga.lax=sna.lax;

	vga.ixh=sna.ixh;
	vga.ixl=sna.ixl;
	vga.iyh=sna.iyh;
	vga.iyl=sna.iyl;

	vga.i=sna.i;
	vga.r=sna.r;

	vga.im=sna.im & 0x02; /* 0 for IM1, 2 for IM2 */

	/* works? how does it know it was IM1 ? */
	if((sna.iff2 & 0x04) != 0)
		vga.im=vga.im | 0x01; 

	vga.sph=sp/256;
	vga.spl=sp%256;
	
	vga.pch=pc/256;
	vga.pcl=pc%256;

	vga.border=sna.border;

	vga.res2=0;
	vga.res3=0;
	vga.res4=0;
	vga.res5=0;
}

void SNZ8_sna_to_z80()
{
	unsigned int addr;
	unsigned int sp;
	unsigned int pc;

	sp=256*sna.sph+sna.spl;
	addr=sp-0x4000;
	pc=image[addr]+256*image[addr+1];
	sp=sp+2;

	z80.f=sna.f;
	z80.a=sna.a;
	z80.b=sna.b;
	z80.c=sna.c;
	z80.d=sna.d;
	z80.e=sna.e;
	z80.h=sna.h;
	z80.l=sna.l;

	z80.fax=sna.fax;
	z80.aax=sna.aax;
	z80.bax=sna.bax;
	z80.cax=sna.cax;
	z80.dax=sna.dax;
	z80.eax=sna.eax;
	z80.hax=sna.hax;
	z80.lax=sna.lax;

	z80.ixh=sna.ixh;
	z80.ixl=sna.ixl;
	z80.iyh=sna.iyh;
	z80.iyl=sna.iyl;

	z80.i=sna.i;
	z80.r=sna.r | 0x080; /* bit 7 is stored somewhere else, always set */
	z80.im=sna.im & 0x03;
	z80.im=z80.im + 0x60; /* fixed normal video/kempston joystick */

	z80.sph=sp/256;
	z80.spl=sp%256;
	
	z80.pch=pc/256;
	z80.pcl=pc%256;

	/* all kinds of stuff put in "data" */
	z80.data=(sna.border & 0x07)*2; 
	if((sna.r & 0x80)!=0) z80.data=z80.data+1; /* here is bit 7 of r */

	/* image is not compressed, compression will be done by the */
	/* z80_write function.                                      */
	z80.data = z80.data & ~0x20; /* reset compressed mode bit to be sure */

	if((sna.iff2 & 0x04) != 0) {
		z80.iff1=0xff;
		z80.iff2=0xff;
	} else {
		z80.iff1=0;
		z80.iff2=0;
	}
}

void SNPR_sna_to_prg(char * n)
{
	unsigned int addr;
	unsigned int sp;
	signed   int i;
	unsigned char * p;

	/* clean header structure first */
	p=(unsigned char *) &prg;
	for(i=0; i < 256; i++)
		p[i]='\0';

	prg.c_0x61=0x61; /* size of image in sectors */
	prg.c_0x35=0x35; /* don't know yet */
	prg.c_0x03=0x03; /* don't know yet */

	sp=256*sna.sph+sna.spl;
	addr=sp-0x4000;

	/* these are on the stack */
	image[addr-1]=sna.a;
	image[addr-2]=sna.f;
	image[addr-3]=sna.r;
	image[addr-4]=sna.iff2;

	sp=sp-4;

	prg.name[0]='\0';
	strncpy(prg.name,n,10);
	prg.name[10]='\0';

	prg.b=sna.b;
	prg.c=sna.c;
	prg.d=sna.d;
	prg.e=sna.e;
	prg.h=sna.h;
	prg.l=sna.l;

	prg.fax=sna.fax;
	prg.aax=sna.aax;
	prg.bax=sna.bax;
	prg.cax=sna.cax;
	prg.dax=sna.dax;
	prg.eax=sna.eax;
	prg.hax=sna.hax;
	prg.lax=sna.lax;

	prg.ixh=sna.ixh;
	prg.ixl=sna.ixl;
	prg.iyh=sna.iyh;
	prg.iyl=sna.iyl;

	prg.i=sna.i;
	prg.iff2=sna.iff2;

	prg.sph=sp/256;
	prg.spl=sp%256;

	/* prg.border=sna.border; */
}

void SNAC_sna_to_ach()
{
	unsigned int addr;
	unsigned int sp;
	unsigned int pc;
	signed   int i;
	unsigned char * p;

	/* clean header structure first */
	p=(unsigned char *) &ach;
	for(i=0; i < 256; i++)
		p[i]='\0';

	sp=256*sna.sph+sna.spl;
	addr=sp-0x4000;
	pc=image[addr]+256*image[addr+1];
	sp=sp+2;

	ach.f=sna.f;
	ach.a=sna.a;
	ach.b=sna.b;
	ach.c=sna.c;
	ach.d=sna.d;
	ach.e=sna.e;
	ach.h=sna.h;
	ach.l=sna.l;

	ach.fax=sna.fax;
	ach.aax=sna.aax;
	ach.bax=sna.bax;
	ach.cax=sna.cax;
	ach.dax=sna.dax;
	ach.eax=sna.eax;
	ach.hax=sna.hax;
	ach.lax=sna.lax;

	ach.ixh=sna.ixh;
	ach.ixl=sna.ixl;
	ach.iyh=sna.iyh;
	ach.iyl=sna.iyl;

	ach.i=sna.i;
	ach.r=sna.r;

	ach.border=sna.border;

	if((sna.iff2 & 0x04) != 0)
		ach.iff2=0xff;
	else
		ach.iff2=0x00;

	ach.im=sna.im;

	ach.sph=sp/256;
	ach.spl=sp%256;
	
	ach.pch=pc/256;
	ach.pcl=pc%256;
}

void SNKG_sna_to_kgb()
{
	unsigned int addr;
	unsigned int sp;
	unsigned int pc;
	signed   int i;
	unsigned char * p;

	/* clean info structure first */
	p = (unsigned char *) &kgb;
	for(i=0; i < 202; i++)
		p[i]='\0';

	/* make some assumptions here */
	kgb.is3_1 = 3;		/* always 3, don't ask me why */
	kgb.colourmode = 1;	/* assume colour */
	kgb.soundmode = 1;	/* assume simple sound */
	kgb.haltmode = 1;	/* assume not in halt mode */

	sp=256*sna.sph+sna.spl;
	addr=sp-0x4000;
	pc=image[addr]+256*image[addr+1];
	sp=sp+2;

	kgb.f=sna.f;
	kgb.a=sna.a;
	kgb.b=sna.b;
	kgb.c=sna.c;
	kgb.d=sna.d;
	kgb.e=sna.e;
	kgb.h=sna.h;
	kgb.l=sna.l;

	kgb.fax=sna.fax;
	kgb.aax=sna.aax;
	kgb.bax=sna.bax;
	kgb.cax=sna.cax;
	kgb.dax=sna.dax;
	kgb.eax=sna.eax;
	kgb.hax=sna.hax;
	kgb.lax=sna.lax;

	kgb.ixh=sna.ixh;
	kgb.ixl=sna.ixl;
	kgb.iyh=sna.iyh;
	kgb.iyl=sna.iyl;

	kgb.i=sna.i;

	kgb.r=sna.r;

	/* kgb.border=sna.border; NOT IN KGB IMAGE! */

	/* Interupt mode is stored in a word in the KGB format. */
	/* Use byte accesses to be CPU independent              */

	switch (sna.im & 0x03) {
	case 0:	kgb.i_mode_h = 0xff;
		kgb.i_mode_l = 0xff;
		break;
	case 2: kgb.i_mode_h = 0;
		kgb.i_mode_l = 1;
		break;
	default:kgb.i_mode_h = 0;
		kgb.i_mode_l = 0;
		break;
	}

	if((sna.iff2 & 0x04) != 0)
		kgb.interruptstatus=0x01;
	else
		kgb.interruptstatus=0x00;

	kgb.sph=sp/256;
	kgb.spl=sp%256;
	
	kgb.pch=pc/256;
	kgb.pcl=pc%256;
}

/* 16K PAGE READ FUNCTION FOR .Z80 FORMAT */

/*
 * Function returns:
 *      0: No more pages
 *	1: Page read
 */
signed int RDPG_z80_read_page(char * s, FILE * fd)
{
	struct	z80_page_s page_info;
	unsigned int	   len;
	unsigned int	   pos;
	signed long	   f_len;

	if(fread(&page_info,z80_pg_size,1,fd)!=1)	{
		return 0;
	}
	len = (256 * page_info.blklen_h) + page_info.blklen_l;

	switch(page_info.page_num) {
	case 0:	fprintf(stderr,"%s - memory page 0 - 48K rom ignored\n",s);
		pos=0x0ffff;
		break;
	case 1:	fprintf(stderr,"%s - memory page 1 - Interface 1 rom ignored\n",s);
		pos=0x0ffff;
		break;
	case 2:	fprintf(stderr,"%s - memory page 2 - basic samram rom ignored\n",s);
		pos=0x0ffff;
		break;
	case 3:	fprintf(stderr,"%s - memory page 3 - monitor samram rom ignored\n",s);
		pos=0x0ffff;
		break;
	case 4:	pos=0x04000;	/* second 16K of RAM area */
		break;
	case 5:	pos=0x08000;	/* third 16K of RAM area */
		break;
	case 6:	fprintf(stderr,"%s - memory page 6 - shadow rom 8000-BFFF ignored\n",s);
		pos=0x0ffff;
		break;
	case 7:	fprintf(stderr,"%s - memory page 7 - shadow rom C000-FFFF ignored\n",s);
		pos=0x0ffff;
		break;
	case 8:	pos=0;		/* first 16K of RAM area */
		break;
	case 9:	fprintf(stderr,"%s - memory page 8 - 128K page 6 ignored\n",s);
		pos=0x0ffff;
		break;
	case 10:fprintf(stderr,"%s - memory page 10 - 128K page 7 ignored\n",s);
		pos=0x0ffff;
		break;
	case 11:fprintf(stderr,"%s - memory page 11 - Multiface rom ignored\n",s);
		pos=0x0ffff;
		break;
	default:fprintf(stderr,"%s - memory page %d - Unknown page number -ignored-\n",s,page_info.page_num);
		pos=0x0ffff;
		break;
	}

	if(pos == (unsigned int) 0x0ffff) {
		/* wrong page, seek over this page */
		if(fseek(fd, (long) len, SEEK_CUR)!=0)
			RERR_read_error(s, fd);
	} else {
		/* Valid 48K page, read and decompress */

		f_len = 0 - ftell(fd);
		Z80D_z80_decompress(fd, pos, 16384);
		f_len = f_len + ftell(fd);

		if((signed long) len != f_len) {
			fprintf(stderr,"Z80 image corrupted, can't convert\n");
			fclose(fd);
			exit(EXIT_FILE_ERROR);
		}
	}
	return 1;
}

/* COMPRESSION/DECOMPRESSION for .Z80 FORMAT */

void Z80D_z80_decompress(FILE * fd, unsigned int start, unsigned int imsize)
{
	signed int    c,j,k;
	unsigned char l;
	unsigned char im;

	j=start;
	while(j<(start+imsize)) {
		c=getc(fd);
		if(c == -1) return;
		im = (unsigned char) c;

		if(im!=0xed) {
			image[j++]=im;
		} else {
			c=getc(fd);
			if(c == -1) return;
			im = (unsigned char) c;
			if(im!=0xed) {
				image[j++]=0xed;
				ungetc(im,fd);
			} else {
				/* fetch count */
				k=getc(fd);
				if(k == -1) return;
				/* fetch character */
				c=getc(fd);
				if(c == -1) return;
				l = (unsigned char) c;
				while(k!=0) {
					image[j++]=l;
					k--;
				}
			}
		}
	}

	if(j!=(start+imsize)) {
		fprintf(stderr,"Z80 image corrupted, can't decompress\n");
		exit(EXIT_FILE_ERROR);
	}
}

int Z80C_z80_compress()
{
#ifdef __TURBOC__
	unsigned char far * comp;
#else
	unsigned char * comp;
#endif
	unsigned int i,j;
	unsigned int num;
	unsigned char c,n;
	unsigned int ed;

	z80_size=(unsigned int) IMSIZE;

	/*
	 * We need an intermediate buffer here, if the compressed image
	 * is bigger than the uncompressed image than the image will
	 * not be compressed to save space (!)
	 */

#ifdef __TURBOC__
	comp=(unsigned char far *) farmalloc((unsigned long)(IMSIZE+0x0100));
#else
	comp=(unsigned char *) malloc((size_t)(IMSIZE+0x0100));
#endif
	if(comp==NULL) {
		printf("Warning: Not enough memory to compress the image, using uncompressed image\n");
		return NOTCOMPRESSED;
	}

	i=0;
	j=0;
	/* ensure 'ed' is not set */
	ed=NO;
	while(i<IMSIZE)	{
		c=image[i];
		i++;
		if(i<IMSIZE) {
			n=image[i];
		} else {
			/* force 'n' to be unequal to 'c' */
			n=c;
			n++;
		}

		if(c!=n) {
			comp[j]=c;
			j++;
			if(c==0xed)
				ed=YES;
			else
				ed=NO;
		} else {
			if(c==0xed) {
				/* two times 0xed - special care */
				comp[j]=0xed;
				j++;
				comp[j]=0xed;
				j++;
				comp[j]=0x02;
				j++;
				comp[j]=0xed;
				j++;
				i++; /* skip second ED */

				/* because 0xed is valid compressed we don't
				   have to watch it! */
				ed=NO;
			} else if(ed==YES) {
				/* can't compress now, skip this double pair */
				comp[j]=c;
				j++;
				ed=NO;	/* 'c' can't be 0xed */
			} else {
				num=1;
				while(i<IMSIZE) {
					if(c!=image[i])
						break;
					num++;
					i++;
					if(num==255)
						break;
				}
				if(num <= 4) {
					/* no use to compress */
					while(num!=0) {
						comp[j]=c;
						j++;
						num--;
					}
				} else {
					comp[j]=0xed;
					j++;
					comp[j]=0xed;
					j++;
					comp[j]=(unsigned char) num;
					j++;
					comp[j]=c;
					j++;
				}
			}
		}

		if(j >= (IMSIZE-4)) {
			/* compressed image bigger or same than original */
#ifdef __TURBOC__
			farfree((void far *) comp);
#else
			free((void *) comp);
#endif
			return NOTCOMPRESSED;
		}
	}
	/* append "end of compressed area" mark */
	comp[j]=0;
	j++;
	comp[j]=0xed;
	j++;
	comp[j]=0xed;
	j++;
	comp[j]=0;
	j++;

	z80_size = j;

	/* copy back */
	i=0;
	j=0;
	while(i<IMSIZE)
		image[i++]=comp[j++];
#ifdef __TURBOC__
	farfree((void far *) comp);
#else
	free((void *) comp);
#endif
	return COMPRESSED;
}

/* SNX to SNA is easy, because the SNX header contains all the stuff in a */
/* SNA header, it's even in the same order. */
void SXSN_snx_to_sna(void)
{
	sna.i = snx.i;
	sna.lax = snx.lax;
	sna.hax = snx.hax;
	sna.eax = snx.eax;
	sna.dax = snx.dax;
	sna.cax = snx.cax;
	sna.bax = snx.bax;
	sna.fax = snx.fax;
	sna.aax = snx.aax;
	sna.l = snx.l;
	sna.h = snx.h;
	sna.e = snx.e;
	sna.d = snx.d;
	sna.c = snx.c;
	sna.b = snx.b;
	sna.iyl = snx.iyl;
	sna.iyh = snx.iyh;
	sna.ixl = snx.ixl;
	sna.ixh = snx.ixh;
	sna.iff2 = snx.iff2;
	sna.r = snx.r;
	sna.f = snx.f;
	sna.a = snx.a;
	sna.spl = snx.spl;
	sna.sph = snx.sph;
	sna.im = snx.im;
	sna.border = snx.border;
}

/* SNA to SNX is easy, because they have identical headers apart from the */
/* extra information present in the SNX header. */
void SNSX_sna_to_snx(void)
{
	snx.signature[0] = 'X';		/* XSNA = eXtended SNA! */
	snx.signature[1] = 'S';
	snx.signature[2] = 'N';
	snx.signature[3] = 'A';
	snx.headerlenhi = (snx_size - 6) << 8;
	snx.headerlenlo = (snx_size - 6) & 0xFF;

	snx.i = sna.i;
	snx.lax = sna.lax;
	snx.hax = sna.hax;
	snx.eax = sna.eax;
	snx.dax = sna.dax;
	snx.cax = sna.cax;
	snx.bax = sna.bax;
	snx.fax = sna.fax;
	snx.aax = sna.aax;
	snx.l = sna.l;
	snx.h = sna.h;
	snx.e = sna.e;
	snx.d = sna.d;
	snx.c = sna.c;
	snx.b = sna.b;
	snx.iyl = sna.iyl;
	snx.iyh = sna.iyh;
	snx.ixl = sna.ixl;
	snx.ixh = sna.ixh;
	snx.iff2 = sna.iff2;
	snx.r = sna.r;
	snx.f = sna.f;
	snx.a = sna.a;
	snx.spl = sna.spl;
	snx.sph = sna.sph;
	snx.im = sna.im;
	snx.border = sna.border;

	/* Set the emulator's switches to some default values. Not sure just
	   what these values mean, but they were like this in most of the SNX
	   files I had to test the program with, so seem safe enough to use */
	snx.emulator_switches[0] = 0;
	snx.emulator_switches[1] = 1;
	snx.emulator_switches[2] = 1;
	snx.emulator_switches[3] = 0x83;
	snx.emulator_switches[4] = 1;
	snx.emulator_switches[5] = 0;
	snx.emulator_switches[6] = 1;
	snx.emulator_switches[7] = 0x41;
	snx.emulator_switches[8] = 0x22;
	snx.emulator_switches[9] = 0xFF;
}

