#include <stdio.h>
#include <stdlib.h>

#include <sys/soundcard.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#ifdef DEBUG_OUTPUT
#  define DOUT(string) fprintf(stderr, "%s\n", string)
#else
#  define DOUT(string)
#endif


int init_soundcard(long sample_rate, int sample_size, int channels)
{
  char *devname = "/dev/dsp";
  int fd;
  int parm;
  int sr;


  DOUT("Opening the DSP device");
    
  if((fd = open(devname, O_RDONLY)) < 0) {
    fprintf(stderr, "Could not open device '%s'; %s\n", 
	    devname, strerror(errno));
    exit(1);
  }


  parm=0x0100008;
  if(ioctl(fd,SNDCTL_DSP_SETFRAGMENT,&parm)>=0)
    DOUT("  Successful");
  else
    DOUT("  Failed");

  DOUT("Resetting the DSP device");
  if(ioctl(fd,SOUND_PCM_RESET,0)>=0) 
    DOUT("  Successful"); 
  else 
    DOUT("  Failed");
  
  DOUT("Syncing the DSP device");
  if(ioctl(fd,SOUND_PCM_SYNC,0)>=0)
    DOUT("  Successful");
  else
    DOUT("  Failed");
  
  DOUT("Setting the DSP device bits per sample");
  if(ioctl(fd,SOUND_PCM_WRITE_BITS,&sample_size)>=0)
    DOUT("  Successful");
  else
    DOUT("  Failed");
  
  DOUT("Setting the DSP device number of channels");
  if(ioctl(fd,SOUND_PCM_WRITE_CHANNELS,&channels)>=0)
    DOUT("  Successful");
  else
    DOUT("  Failed");
  
   /* Verify the results */
   if((sample_size!=16) && (sample_size!=8))
   {
      fprintf(stderr, 
	      "DSP error: returned a sample size of %d bits.\n",sample_size);
      exit(1);
   }
/*  fprintf(stderr, "Sample size: %i\n", sample_size); */


  sr = sample_rate;
  DOUT("Syncing the DSP device");
  ioctl(fd,SOUND_PCM_SYNC,0);
  
  DOUT("Setting the DSP device sampling rate");
  ioctl(fd,SOUND_PCM_WRITE_RATE,&sr);
  sample_rate=sr;
  if(sample_rate<1000)
    {
      printf("DSP error: returned a sample rate of %ld samples/second.\n",
             sample_rate);
      exit(1);
    }

/*  fprintf(stderr, "Sample rate: %li\n", sample_rate); */

  return fd;
}


int main(int argc, char *argv[])
{
  int sample_size = 8;
  long sample_rate = 8000;
  int channels = 1;

  FILE *fp;
  char *filename;

  int snd;

  int to_quit, recordig;

  int records;
  int rd;
  int at;


#define BUFSIZE 1024

  unsigned char buf[BUFSIZE];

  if(argc < 2) {
    fprintf(stderr, 
	    "usage: %s filename [sample_rate] [sample_size]\n", 
	    argv[0]);
    exit(1);
  }

  filename = argv[1];

  if(strcmp(filename, "-") == 0) fp = stdout;
  else if((fp = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "%s: Could not open `%s', %s\n", 
	    argv[0], filename, strerror(errno));
    exit(1);
  }

  
  if(argc > 2) {
    sample_rate = atol(argv[2]);
    if(sample_rate < 4000 || sample_rate > 41000) {
      fprintf(stderr, "%s: Illegal sample rate: %s\n", argv[0], argv[2]);
      exit(1);
    }
  }

  if(argc > 3) {
    sample_size = atol(argv[3]);
    if(sample_size != 8 && sample_size != 16) {
      fprintf(stderr, "%s: Illegal sample size: %s\n", argv[0], argv[3]);
      exit(1);
    }
  }


  snd = init_soundcard(sample_rate, sample_size, channels);


  to_quit = 0;
  recordig = 1;
  records = 0;


/*  fprintf(stderr, "\nStart!\n"); */
  
  do {
    for(at = 0; at < BUFSIZE; at += rd) {
      rd = read(snd, buf + at, BUFSIZE - at);
      if(rd < 0) {
	fprintf(stderr, "\nError on soundcard: %s\n", strerror(errno));
	exit(1);
      }
    }
    fwrite(buf, 1, BUFSIZE, fp);
    records++;
/*
     fprintf(stderr, "records written: %4i       \r", records); 
    fflush(stderr);
*/
  } while(!to_quit);
    
/*  fprintf(stderr, "\nStop!\n"); */
  fclose(fp);

  return 0;
}

