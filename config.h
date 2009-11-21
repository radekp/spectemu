/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader 2.13.  */

/* Define this on i386 architectures if the C compiler generates symbols
   beginning with underscores, eg. on old aout versions of Linux */
/* #undef AOUT_FORMAT */

/* Define this to enable running in background on the Linux console.
   Works only with SVGALIB 1.2.11 or newer */
/* #undef RUN_IN_BACKGROUND */

/* Define this if Xlib has the MITSHM extension */
#define HAVE_MITSHM 1

/* Define this if program can query MITSHM extension */
#define HAVE_SHMQUERY 1

/* Define for the XFree86 X server */
/* #undef HAVE_XF86VIDMODE */

/* Define this if you have the readline library */
/* #undef HAVE_READLINE */

/* Define this to use the C version of the program insead of the 
   i386 assembly. Define this on non intel machines */
#define Z80C 1

/* Always define this for the spectrum emulator. */
#define SPECT_MEM 1

/* Define if sound driver is available. */
#define HAVE_SOUND 1

/* Define if sound driver is Open Sound System (OSS) */
#define OSS_SOUND 1

/* Define if sound driver is SUN */
/* #undef SUN_SOUND */

/* Define if sound driver is IRIX */
/* #undef IRIX_SOUND */

/* Define if you want to use PC speaker sound */
/* #undef PCSPEAKER_SOUND */

/* Define if you want to use the Linux joystick driver */
#define LINUX_JOYSTICK 1

/* Define this to use the inline intel assembly sections */
/* #undef I386_ASM */


/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */
