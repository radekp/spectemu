#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define MAXLEN 256

typedef unsigned char byte;

static int read_tape_line(FILE *tapefp, int line, int max, byte *flag)
{
  static char buf[MAXLEN];
  int iflag;
  int res;
  int i;

  do {
    if(fgets(buf, MAXLEN, tapefp) == NULL) return -1;
    
    res = sscanf(buf, "%d %x", &i, &iflag);
    if(max >= 0 && res == 2 && i > max) return -2;

  } while(res < 2 || i < line);

  *flag = (byte) iflag;
  fprintf(stderr, "%s", buf);

  return i;
}


int main(int argc, char *argv[])
{
  char filename[MAXLEN];

  FILE *tap, *spt, *blk;
  int len, n, c;
  int from, to;
  int line;
  byte flag;
  byte parity;

  if(argc < 2) {
    fprintf(stderr, "usage: %s sptfile [tapfile [from to]]\n", argv[0]);
    exit(1);
  }

  strcpy(filename, argv[1]);
  strcat(filename, ".spt");
  spt = fopen(filename, "rt");
  if(spt == NULL) {
    fprintf(stderr, "Could not open file %s. %s\n", 
	    filename, strerror(errno));
    exit(1);
  }

  if(argc < 3) strcpy(filename, argv[1]);
  else strcpy(filename, argv[2]);
  strcat(filename, ".tap");
  tap = fopen(filename, "wb");
  if(tap == NULL) {
    fprintf(stderr, "Could not open file %s. %s\n", 
	    filename, strerror(errno));
    exit(1);
  }
  
  if(argc < 4) from = 0;
  else from = atoi(argv[3]);
  
  if(argc < 5) to = -1;
  else to = atoi(argv[4]);
  
  line = from;

  for(;;) {
    line = read_tape_line(spt, line, to, &flag);
    if(line == -1) {
      fprintf(stderr, "End of tapefile\n");
      break;
    }
    if(line == -2) break;

    sprintf(filename, "%s.%03i", argv[1], line);
    blk = fopen(filename, "rb");

    if(blk == NULL) {
      fprintf(stderr, "Could not open file %s. %s\n", 
	      filename, strerror(errno));
      exit(1);
    }

    fseek(blk, 0, SEEK_END);
    len = ftell(blk) + 2;
    if(len > 0xFFFF) len = 0xFFFFF;
    rewind(blk);

    putc(len & 0xFF, tap);
    putc(len >> 8, tap);
    putc(flag, tap);
    
    parity = flag;

    for(n = 0; n < len - 2; n++) {
      c = getc(blk);
      if(c == EOF) {
	fprintf(stderr, "Warning: %s EOF too early\n", filename);
	c = 0;
      }
      parity = parity ^ (byte) c;

      putc(c, tap);
    }
    putc(parity, tap);

    fclose(blk);
  }

  fclose(spt);
  fclose(tap);

  return 0;
}
