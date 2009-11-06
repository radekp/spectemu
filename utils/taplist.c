#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

int main(int argc, char *argv[])
{
  int c;
  int len;
  int parity;
  int end;
  byte header[18];
  char filename[11];
  int filesize;
  int filetype;
  int ctr;

  int i;
  
  filename[10] = '\0';

  if(argc > 1) {
    fprintf(stderr, "usage: %s < tapfile\n", argv[0]);
    exit(1);
  }
  
  end = 0;
  ctr = 0;

  while(!end) {
    c = getchar();
    if(c == EOF) {
      end = 1;
      continue;
    }

    len = c;
    c = getchar();
    if(c == EOF) {
      end = -1;
      continue;
    }

    len += (c << 8);
    parity = 0;

    for(i = 0; i < len; i++) {
      c = getchar();
      if(c == EOF) {
	end = -1;
	break;
      }
      parity = parity ^ c;

      if(i < 18) header[i] = c;
    }

    printf("%03d", ctr);
    if(i >= 1) {
      printf(" %02X %5d %3s", header[0], len-2, 
	     parity == 0 ? "OK" : "ERR");
      if(header[0] == 0 && len - 2 >= 17) {
	filetype = header[1];
	strncpy(filename, header+2, 10);
	filesize = header[12] + (header[13] << 8);

	printf("  %02X %10s %5i", filetype, filename, filesize);
      }
    }
    printf("\n");
 
    ctr++;
  }

  if(end < 0) {
    fprintf(stderr, "Error: Incomplete segment in tape file\n");
    return 1;
  }

  return 0;
}
