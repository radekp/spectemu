#ifndef SPCONV_H
#define SPCONV_H

/*
 * Exit-codes used by the program:
 *
 * EXIT_OK:          The program finished without problems:
 *
 * EXIT_USAGE:       The program was called incorrectly:
 *                   - Wrong number of parameters
 *                   - Wildcards on the command-line
 *                   - Input and output are of the same type
 *                     (not for .SP and .Z80)
 *
 * EXIT_READ_ERROR:  There was a problem while reading the input file:
 *                   - Error while reading the input file
 *                   - Error while fseeking the input file
 *
 * EXIT_WRITE_ERROR: There was a problem while creating the output file:
 *                   - Error while writing the output file
 *                   - Error with opening/reading the spectrum.rom needed
 *                     for the output file.
 *
 * EXIT_FILE_ERROR:  There was a problem with the input or output file format:
 *                   - Input file does not exist
 *                   - Type of input or output file not known
 *                   - Input file is:
 *                     - A newer version of an existing type
 *                     - Corrupt
 *                     - Not in the expected format (RAW and new .SP types)
 *                     - Not a 48K spectrum file (in .Z80 files)
 *                   - Input file snapshot was taken while the interface-1 was
 *                     paged in (in .Z80 files). The other file formats can not
 *                     cope with this (including the version 1.45 .Z80 files).
 *                   - Undocumented page number in .Z80 files (file corrupt)
 *                   - Decompression of .Z80 file failed (file corrupt)
 */

#define EXIT_OK		   0 /* No error */
#define EXIT_USAGE	   1 /* Problem with the command-line call */
#define EXIT_READ_ERROR	   2 /* Error while reading the input file */
#define EXIT_WRITE_ERROR   3 /* Error while writing the output file */
#define EXIT_FILE_ERROR	   4 /* Problem with the type of the input file */

/* Function Prototypes */

/* SPECIAL FUNCTIONS */
const char * DTYP_determine_type(char * filename, signed int * type);
signed int DIRC_direct_convert(signed int type, char * infile, char * outfile);

/* ERROR HANDLING FUNCTIONS */
void USAG_usage(void);
void RERR_read_error(char * s, FILE * fd);
void WERR_write_error(char * s, FILE * fd);

/* I/O routines */
void RGEN_read_generic(char * s, void * header, size_t h_size);
void WGEN_write_generic(char * s, void * header, size_t h_size);
void RERR_read_error(char * s, FILE * fd);
void WERR_write_error(char * s, FILE * fd);

void RSNA_read_sna(char * s);
void WSNA_write_sna(char * s);
void RVGH_read_vgaspec_header(char * s);
void RVGA_read_vgaspec(char * s);
void WVGA_write_vgaspec(char * s);
void ROVG_read_old_vgaspec(char * s);
void WOVG_write_old_vgaspec(char * s);
void RRAW_read_raw(char * s);
void RZ80_read_z80(char * s);
void WZ80_write_z80(char * s);
void RPRG_read_prg(char * s);
void WPRG_write_prg(char * s);
void RACH_read_ach(char * s);
void WACH_write_ach(char * s);
void RKGB_read_kgb(char * s);
void WKGB_write_kgb(char * s);
void RSNX_read_snx(char * s);
void WSNX_write_snx(char * s);

/* Conversion routines */
void VGSN_vgaspec_to_sna(void);
void RASN_raw_to_sna(void);
void Z8SN_z80_to_sna(void);
void PRSN_prg_to_sna(void);
void ACSN_ach_to_sna(void);
void KGSN_kgb_to_sna(void);
void SXSN_snx_to_sna(void);

void SNVG_sna_to_vgaspec(void);
void SNZ8_sna_to_z80(void);
void SNPR_sna_to_prg(char * n);
void SNAC_sna_to_ach(void);
void SNKG_sna_to_kgb(void);
void SNSX_sna_to_snx(void);

/* 16K page read function for Z80 format */
signed int RDPG_z80_read_page(char * s, FILE * fd);

/* Compression/Decompression for Z80 */
void Z80D_z80_decompress(FILE * fd, unsigned int start, unsigned int length);
signed int  Z80C_z80_compress(void);

/* Compression/Decompression for SNX */
void SNXD_snx_decompress(FILE * fd, unsigned int start, unsigned int length);
signed int SNXC_snx_compress(void);

/* File header for file from RS232 Link - used for RAW conversion */
union header_u {
	signed char in[9];
	struct {
		signed char type;
		unsigned int  length;
		unsigned int  start;
		signed char var;
		signed char res1;
		signed int  line;
	} header;
} h;
#define hdr_size 9		/* sizeof(h)=9 */

/* The contents of the header for a RAW file */
char expect[]={
	0x03,		/* type CODE */
	0x00,0xc0,	/* image size */
	0x00,0x40,	/* image start */
	0xff,		/* var */
	0xff,		/* res1 */
	0xff,0xff	/* line */
};

/* Register storage structures for the various types */

struct snx_s {
	char signature[4];
	unsigned char headerlenhi,headerlenlo;
	unsigned char i;
	unsigned char lax;
	unsigned char hax;
	unsigned char eax;
	unsigned char dax;
	unsigned char cax;
	unsigned char bax;
	unsigned char fax;
	unsigned char aax;
	unsigned char l;
	unsigned char h;
	unsigned char e;
	unsigned char d;
	unsigned char c;
	unsigned char b;
	unsigned char iyl;
	unsigned char iyh;
	unsigned char ixl;
	unsigned char ixh;
	unsigned char iff2;
	unsigned char r;
	unsigned char f;
	unsigned char a;
	unsigned char spl;
	unsigned char sph;
	unsigned char im;
	unsigned char border;
	unsigned char emulator_switches[10];
};
#define snx_size 43		/* sizeof(struct snx_s)=43 */

struct sna_s {
	unsigned char i;
	unsigned char lax;
	unsigned char hax;
	unsigned char eax;
	unsigned char dax;
	unsigned char cax;
	unsigned char bax;
	unsigned char fax;
	unsigned char aax;
	unsigned char l;
	unsigned char h;
	unsigned char e;
	unsigned char d;
	unsigned char c;
	unsigned char b;
	unsigned char iyl;
	unsigned char iyh;
	unsigned char ixl;
	unsigned char ixh;
	unsigned char iff2;
	unsigned char r;
	unsigned char f;
	unsigned char a;
	unsigned char spl;
	unsigned char sph;
	unsigned char im;
	unsigned char border;
};
#define sna_size 27		/* sizeof(struct sna_s)=27 */

struct vga_s {
/*00*/	unsigned char S;
/*01*/	unsigned char P;
/*02*/	unsigned char len_l;
/*03*/	unsigned char len_h;
/*04*/	unsigned char start_l;
/*05*/	unsigned char start_h;
/*06*/	unsigned char c;
/*07*/	unsigned char b;
/*08*/	unsigned char e;
/*09*/	unsigned char d;
/*0A*/	unsigned char l;
/*0B*/	unsigned char h;
/*0C*/	unsigned char f;
/*0D*/	unsigned char a;
/*0E*/	unsigned char ixl;
/*0F*/	unsigned char ixh;
/*10*/	unsigned char iyl;
/*11*/	unsigned char iyh;
/*12*/	unsigned char cax;
/*13*/	unsigned char bax;
/*14*/	unsigned char eax;
/*15*/	unsigned char dax;
/*16*/	unsigned char lax;
/*17*/	unsigned char hax;
/*18*/	unsigned char fax;
/*19*/	unsigned char aax;
/*1A*/	unsigned char r;
/*1B*/	unsigned char i;
/*1C*/	unsigned char spl;
/*1D*/	unsigned char sph;
/*1E*/	unsigned char pcl;
/*1F*/	unsigned char pch;
/*20*/	unsigned char res2;
/*21*/	unsigned char res3;
/*22*/	unsigned char border;
/*23*/	unsigned char res4;
/*24*/	unsigned char im;
/*25*/	unsigned char res5;
};
#define vga_size 38		/* sizeof(struct vga_s)=38 */

struct z80_s {
/*00*/	unsigned char a;
/*01*/	unsigned char f;
/*02*/	unsigned char c;
/*03*/	unsigned char b;
/*04*/	unsigned char l;
/*05*/	unsigned char h;
/*06*/	unsigned char pcl;
/*07*/	unsigned char pch;
/*08*/	unsigned char spl;
/*09*/	unsigned char sph;
/*0A*/	unsigned char i;
/*0B*/	unsigned char r;
/*0C*/	unsigned char data;
/*0D*/	unsigned char e;
/*0E*/	unsigned char d;
/*0F*/	unsigned char cax;
/*10*/	unsigned char bax;
/*11*/	unsigned char eax;
/*12*/	unsigned char dax;
/*13*/	unsigned char lax;
/*14*/	unsigned char hax;
/*15*/	unsigned char aax;
/*16*/	unsigned char fax;
/*17*/	unsigned char iyl;
/*18*/	unsigned char iyh;
/*19*/	unsigned char ixl;
/*1A*/	unsigned char ixh;
/*1B*/	unsigned char iff1;
/*1C*/	unsigned char iff2;
/*1D*/	unsigned char im;
/* Extended 2.01 and 3.0 header, flagged with PC=0 */
/*1E*/	unsigned char h2_len_l;
/*1F*/	unsigned char h2_len_h;
/*20*/  unsigned char n_pcl;
/*21*/  unsigned char n_pch;
/*22*/  unsigned char hardware;
/*23*/  unsigned char samram;
/*24*/  unsigned char if1_paged;
/*25*/  unsigned char r_ldir_emu;
/*26*/  unsigned char last_out;
/*27*/  unsigned char sound_reg[16];
/* Continues with extended 3.0 header, but this part is not used anyway */
};
#define z80_145_size 0x1e	/* length of z80 V1.45 header */
#define z80_201_ext_size 23	/* length of extended z80 V2.01 header */
#define z80_300_ext_size 54	/* length of extended z80 V3.0  header */

struct z80_page_s {
/*00*/	unsigned char blklen_l;
/*01*/	unsigned char blklen_h;
/*02*/	unsigned char page_num;
};
#define z80_pg_size 3		/* sizeof(struct z80_page_s)=3 */

struct prg_s {
/*00*/	signed char name[10];
/*0A*/	signed char nullbyte;
/*0B*/	unsigned char c_0x61;
/*0C*/	unsigned char c_0x35;
/*0D*/	unsigned char c_0x03;
/*0E*/  unsigned char c_0x00[0xdc - 0x0e];
/*DC*/	unsigned char iyl;
/*DD*/	unsigned char iyh;
/*DE*/	unsigned char ixl;
/*DF*/	unsigned char ixh;
/*E0*/	unsigned char eax;
/*E1*/	unsigned char dax;
/*E2*/	unsigned char cax;
/*E3*/	unsigned char bax;
/*E4*/	unsigned char lax;
/*E5*/	unsigned char hax;
/*E6*/	unsigned char fax;
/*E7*/	unsigned char aax;
/*E8*/	unsigned char e;
/*E9*/	unsigned char d;
/*EA*/	unsigned char c;
/*EB*/	unsigned char b;
/*EC*/	unsigned char l;
/*ED*/	unsigned char h;
/*EE*/	unsigned char iff2;
/*EF*/	unsigned char i;
/*F0*/	unsigned char spl;
/*F1*/	unsigned char sph;
/*F2*/	unsigned char filler[0x0e];
};
#define prg_size 256		/* sizeof(struct prg_s)=256 */

struct ach_s {
/*00*/	unsigned char a;
/*01*/	unsigned char fill1[3];
/*04*/	unsigned char f;
/*05*/	unsigned char fill2[3];
/*08*/	unsigned char b;
/*09*/	unsigned char fill3[3];
/*0C*/	unsigned char c;
/*0D*/  unsigned char fill4[3];
/*10*/	unsigned char d;
/*11*/	unsigned char fill5[3];
/*14*/	unsigned char e;
/*15*/	unsigned char fill6[3];
/*18*/	unsigned char h;
/*19*/	unsigned char fill7[3];
/*1C*/	unsigned char l;
/*1D*/	unsigned char fill8[3];
/*20*/	unsigned char pcl;
/*21*/	unsigned char pch;
/*22*/	unsigned char fill9[2];
/*24*/  unsigned char fill10[4];
/*28*/  unsigned char spl;
/*29*/  unsigned char sph;
/*2A*/	unsigned char fill11[0x94 - 0x2A];
/*94*/	unsigned char r;
/*95*/	unsigned char fill12[0x9C - 0x95];
/*9C*/	unsigned char border;
/*9D*/	unsigned char fill13[0xa4 - 0x9D];
/*A4*/  unsigned char im;
/*A5*/	unsigned char fill14[0xbe - 0xa5];
/*BE*/  unsigned char i;
/*BF*/  unsigned char iff2;
/*C0*/	unsigned char fill15[0xec - 0xc0];
/*EC*/	unsigned char aax;
/*ED*/	unsigned char fax;
/*EE*/	unsigned char fill16[2];
/*F0*/	unsigned char bax;
/*F1*/	unsigned char cax;
/*F2*/	unsigned char fill17[2];
/*F4*/	unsigned char dax;
/*F5*/	unsigned char eax;
/*F6*/	unsigned char hax;
/*F7*/	unsigned char lax;
/*F8*/  unsigned char ixl;
/*F9*/  unsigned char ixh;
/*FA*/  unsigned char fill18[2];
/*FC*/  unsigned char iyl;
/*FD*/  unsigned char iyh;
/*FE*/	unsigned char fill19[2];
};
#define ach_size 256		/* sizeof(struct ach_s)=256 */

struct kgb_s {
/*00*/	unsigned char unused_1[0x84 - 0x00];
/*84*/	unsigned char settings[0x8E - 0x84];
/*8E*/	unsigned char interruptstatus;
/*8F*/	unsigned char is0_1;
/*90*/	unsigned char is3_1;
/*91*/	unsigned char colourmode;
/*92*/	unsigned char is0_2;
/*93*/	unsigned char is0_3;
/*94*/	unsigned char is0_4;
/*95*/	unsigned char is0_5;
/*96*/  unsigned char b;
/*97*/  unsigned char c;
/*98*/  unsigned char bax;
/*99*/  unsigned char cax;
/*9A*/  unsigned char d;
/*9B*/  unsigned char e;
/*9C*/  unsigned char dax;
/*9D*/  unsigned char eax;
/*9E*/  unsigned char h;
/*9F*/  unsigned char l;
/*A0*/  unsigned char hax;
/*A1*/  unsigned char lax;
/*A2*/  unsigned char ixh;
/*A3*/  unsigned char ixl;
/*A4*/  unsigned char iyh;
/*A5*/  unsigned char iyl;
/*A6*/  unsigned char i;
/*A7*/  unsigned char r;
/*A8*/  unsigned char is0_6;
/*A9*/	unsigned char is0_7;
/*AA*/  unsigned char is0_8;
/*AB*/  unsigned char aax;
/*AC*/  unsigned char is0_9;
/*AD*/  unsigned char a;
/*AE*/  unsigned char is0_10;
/*AF*/  unsigned char fax;
/*B0*/  unsigned char is0_11;
/*B1*/  unsigned char f;
/*B2*/  unsigned char is0_12;
/*B3*/  unsigned char is0_13;
/*B4*/  unsigned char pch;
/*B5*/  unsigned char pcl;
/*B2*/  unsigned char is0_14;
/*B3*/  unsigned char is0_15;
/*B8*/  unsigned char sph;
/*B9*/  unsigned char spl;
/*BA*/  unsigned char is0_16;
/*BB*/  unsigned char soundmode;
/*BC*/  unsigned char is0_17;
/*BD*/  unsigned char haltmode;
/*BE*/  unsigned char i_mode_h;
/*BF*/  unsigned char i_mode_l;
/*C0*/	unsigned char unused_2[0xCA - 0xC0];
};
#define kgb_size 202		/* sizeof(struct kgb_s)=202 */

/* Constants */
#define IMSIZE 49152L

/* Constants for Z80 Compression/Decompression */
#define NOTCOMPRESSED	0
#define COMPRESSED	0x20
#define NO		0
#define YES		1

/* Constants for SNX Compression/Decompression */
#define SNX_COMPRESSED		0xFF
#define SNX_UNCOMPRESSED	0

/* Contstants for the implemented formats in this version */
#define	RAW		1
#define SNA		2
#define	SP		3
#define	Z80		4
#define	PRG		5
#define	ACH		6
#define KGB		7
#define SNX		8
#define	UNKNOWN		9

#endif /* SPCONV_H */
