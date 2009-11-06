#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SPT_EXT "spt"

const int SYNCC = 512;


const int BUFSIZE = 1024;

const int upth = 1;
const int downth = -1;

inline double sqr(double x) { return x * x; }


class EdgeDetect {
private:
  FILE *infile;

  signed char buf[BUFSIZE];

  int sl, dl;
  int pos;
  int lastdpos;
  int lastdtype;

  int lastpos;


  int lastdist;

  int bufp;
  int bufoff;


public:
  EdgeDetect(FILE *fp);
  
  int edge(int &dist, int &dist2, int &dir);
  void store_buffer(FILE *fp);
};

EdgeDetect::EdgeDetect(FILE *fp) 
{
  int i;

  infile = fp;

  for(i = 0; i < BUFSIZE; i++) buf[i] = 0;
  
  bufp = 0;
  bufoff = 0;

  lastdtype = 0;
  lastdpos = 0;
  sl = 0;
  lastpos = 0;
  dl = 0;

  pos = 0;
}

int EdgeDetect::edge(int &dist, int &dist2, int &dir)
{
  int c;
  int edge;
  int sn, dn;
  int minv, maxv;
  int thrs;
  int nowth;

  for(; (c = getc(stdin)) != EOF; pos++) {
    edge = 0;
    sn = c - 128;
    buf[bufp] = sn;

    dn = sn - sl;
    
    if(pos - lastdpos > 2) {
      if(lastdtype != 1 && dl < upth && dn >= upth) {
	edge = 1;
	lastdtype = 1;
      }
      
      if(lastdtype != -1 && dl > downth && dn <= downth) {
	edge = 1;
	lastdtype = -1;
      }
    }
      
    if(edge) {
      if(pos - lastdpos < BUFSIZE) {
	int bufb = (lastdpos - bufoff + BUFSIZE) % BUFSIZE;
	
	minv = buf[bufb];
	maxv = buf[bufp];
	thrs = (minv + maxv) / 2;
	
	for(nowth = lastdpos; nowth < pos; 
	    bufb = (bufb + 1) % BUFSIZE, nowth++) {
	  if(lastdtype == 1 && buf[bufb] <= thrs) break;
	  if(lastdtype == -1 && buf[bufb] >= thrs) break;
	}
      }
      else nowth = pos;

      dist = nowth - lastpos;

      lastpos = nowth;
      lastdpos = pos;

      dist2 = dist + lastdist;
      lastdist = dist;
      dir = -1 * lastdtype;
      
      return 1;
    }

    bufp++;
    if(bufp == BUFSIZE) {
      bufp = 0;
      bufoff += BUFSIZE;
    }
      
    sl = sn;
    dl = dn;

  }

  return 0;
}

void EdgeDetect::store_buffer(FILE *fp)
{
  int i;
  
  for(i = (bufp + 1) % BUFSIZE; i != bufp; i = (i + 1) % BUFSIZE)
    putc((int) buf[i] + 128, fp);
}



const char *statename[] = {
  "NOTHING",
  "SCANNING",
  "SYNC",
  "START", 
  "DATA"
};


class SegmentRead : private EdgeDetect {
public:
  SegmentRead(FILE *fp);
  
  int n_bytes(int n, unsigned char *buf, int &flag, int& bytenum);

};

SegmentRead::SegmentRead(FILE *fp)
: EdgeDetect(fp)
{
}

int SegmentRead::n_bytes(int n, unsigned char *buf, int &flags, int &bytenum)
{
  int dist, dist2, dir;
  int state;
  int laststate;
  int synccount;
  int syncdir;
  int bitnum;
  int databyte;
  
  int bitval;
  int parity;
  int firstbyte;

  int retval = -4;
  
  state = 0;
  laststate = -1;

  bytenum = -1;

  int syncsum;
  double syncvar;

  int b1sum, b0sum;
  double b1var, b0var;
  int b1ctr, b0ctr;

  b0sum = b1sum = 0;
  b0var = b1var = 0;
  b0ctr = b1ctr = 0;


  const int MINSYNC = 25;
  const int MAXSYNC = 50;
  const int STARTMAX = 14;
  const int STARTMIN = 2;

  while(edge(dist, dist2, dir)) {
    if(state == 0) {
      if(dist2 >= MINSYNC && dist2 <= MAXSYNC) {
	state = 1;
	synccount = 0;
	syncsum = 0;
	syncvar = 0;
      }
    }
    else if(state == 1) {
      if(dist2 >= MINSYNC && dist2 <= MAXSYNC) {
	synccount++;
	syncsum += dist2;
	syncvar += sqr((double) dist2 - 
		       ((double) syncsum / (double) synccount));
	if(synccount >= SYNCC) state = 2;
      }
      else state = 0;
    }
    else if(state == 2) {
      if(dist > STARTMIN && dist < STARTMAX) {
	fprintf(stderr, "syncave: %.2f\n", 
		(double) syncsum / (double) synccount);
	state = 3;
      }
      else if(dist2 < MINSYNC || dist2 > MAXSYNC) {
	fprintf(stderr, "FALSE SYNC, dist: %i, dist2: %i\n", 
		dist, dist2);
	
	state = 0;
      }
      else {
	synccount++;
	syncsum += dist2;
	syncvar += sqr((double) dist2 - 
		       ((double) syncsum / (double) synccount));
      }
    }
    else if(state == 3) {
      fprintf(stderr, "startdists: %i, %i\n", dist2 - dist, dist);
      if(dist > STARTMIN && dist < STARTMAX) {
	state = 4;
	bitnum = 0;
	bytenum = -1;
	parity = 0;
	firstbyte = 1;
	databyte = 0;
	syncdir = dir;

	b0sum = b1sum = 0;
	b0var = b1var = 0;
	b0ctr = b1ctr = 0;
	
      }
      else {
	fprintf(stderr, "FALSE START\n");
	state = 2;
      }
    }
    else if(state == 4) {
      if(dir == syncdir) {
	
	if(dist2 > 5 && dist2 < 45) {
	  
	  databyte <<= 1;
	  if(dist2 > 22) {
	    bitval = 1;
	    b1ctr++;
	    b1sum+=dist2;
	    b1var+=sqr((double) dist2 - ((double) b1sum / (double) b1ctr));
	  }
	  else {
	    bitval = 0;
	    b0ctr++;
	    b0sum+=dist2;
	    b0var+=sqr((double) dist2 - ((double) b0sum / (double) b0ctr));
	  }
	  databyte += bitval;
	  
	  bitnum++;
	  
	  if(bitnum == 8) {
	    bitnum = 0;
	    
	    parity ^= databyte;
	    
	    if(firstbyte) {
	      if(flags >= 0 && flags != databyte) {
		state = 0;
		fprintf(stderr, "searching flag: %i, found flag: %i\n", 
			flags, databyte);
		continue;
	      }
	      if(flags == -2 && databyte == 0 && n < 0) n = 17;
	      flags = databyte;
	      fprintf(stderr, "found flag: %i\n", flags);
	      firstbyte = 0;
	      bytenum = 0;
	    }
	    else if(bytenum < n || n < 0) {
	      buf[bytenum] = databyte;
	      bytenum++;
	    
	      if(bytenum % 100 == 0) 
		
		fprintf(stderr, "pos: %5i  b1ave: %4.1f  b0ave: %4.1f  \r",
			bytenum, 
			b1ctr ? (double) b1sum / (double) b1ctr : 0.0, 
			b0ctr ? (double) b0sum / (double) b0ctr : 0.0);
	    }
	    else {
	      if(parity == 0) retval = 0;
	      else retval = -1;
	      break;
	    }
	    databyte = 0;
	  }
	}
	else {
	  if(n >= 0 || parity != 0) {
	    fprintf(stderr, "SIGNAL LOST: dist: %i, dist2: %i, bytenum: %i, bitnum: %i\n", 
		    dist, dist2, bytenum, bitnum);
	    
	    FILE *fp = fopen("bufstor", "w");
	    if(fp != NULL) store_buffer(fp);
	    fclose(fp);
	  }

	  if(n < 0) {
	    if(parity == 0) {
	      bytenum--;
	      retval =  1;
	    }
	    else retval =  -3;
	  }
	  else retval =  -2;

	  break;
	}
      }
    }
  
    if(state != laststate) {
      if(state >= 2) 
	fprintf(stderr, "state: %s\n", statename[state]);
      laststate = state;
    }
  }

  fprintf(stderr, "pos: %5i  b1ave: %4.1f  b0ave: %4.1f  \r",
	  bytenum, 
	  b1ctr ? (double) b1sum / (double) b1ctr : 0.0, 
	  b0ctr ? (double) b0sum / (double) b0ctr : 0.0);

  return retval; 
}

const char *sperror(int status)
{
  switch(status) {
  case 1: return "end found, parity OK";
  case 0: return "parity OK";
  case -1: return "parity error";
  case -2: return "synchronisation lost";
  case -3: return "end found, parity error";
  case -4: return "end of tape";
  }
  
  return "unknown error";
}


int main(int argc, char *argv[])
{
  
  SegmentRead sr(stdin);

  unsigned char buf[65536];

  int flags = -1;
  int nextflags;
  int status;
  int numbytes;
  int num = -1;
  int isheader;
  
  char filename[11];
  filename[10] = 0;

  int filetype;
  int filesize;
  
  int segc = 0;

  FILE *tapefile;
  FILE *datfile;

  char datfilename[256];
  char *tapefilename;

  if(argc < 2) tapefile = NULL;
  else {
    tapefilename = argv[1];
    sprintf(datfilename, "%s.%s", tapefilename, SPT_EXT);

    tapefile = fopen(datfilename, "w");
    if(tapefile == NULL) {
      fprintf(stderr, "Could not open tape directory file %s\n", 
	      datfilename);
      exit(1);
    }
  }

  while(1) { 
    status = sr.n_bytes(num, buf, flags, numbytes);
    
    if(status < 0) 
      fprintf(stderr, "\nError reading file segment: %s\n", sperror(status));
    else 
      fprintf(stderr, "\nStatus: %s\n", sperror(status));

    if(status == -4) break;

    fprintf(stderr, "flags: %i, numbytes: %i\n", flags, numbytes);
    
    if(flags == 0 /*&& numbytes == 17*/) {
      isheader = 1;
      fprintf(stderr, "   HEADER\n");

      filetype = buf[0];

      fprintf(stderr, "file type: %i\n", filetype);
      
      strncpy(filename, (char *) &buf[1], 10);

      fprintf(stderr, "file name: `%s'\n", filename);
      
      filesize = buf[11] + ((buf[12]) << 8);
//      num = filesize;
//      nextflags = 255;
      num = -1;
      nextflags = -1;
      
      fprintf(stderr, "file size: %i bytes\n", filesize);

      fprintf(stderr, "misc info: %02X %02X %02X %02X\n", 
	      buf[13], buf[14], buf[15], buf[16]);
    }
    else {
      isheader = 0;
      num = -1;
      nextflags = -1;
    }
    
    fprintf(stderr, "\n");

    if(tapefile != NULL) {

      fprintf(tapefile, "%03i %02X %5i %3s", 
	      segc, flags, numbytes, status < 0 ? "ERR" : "OK ");
      
      if(isheader) 
	fprintf(tapefile, "  %10s %5i %02X", filename, filesize, filetype);
      
      fprintf(tapefile, "\n");
      fflush(tapefile);
      
      sprintf(datfilename, "%s.%03i", tapefilename, segc);
      
      datfile = fopen(datfilename, "w");
      if(datfile == NULL) {
	fprintf(stderr, "Could not open data file %s\n", datfilename);
	exit(1);
      }
      
      fwrite(buf, 1, numbytes, datfile);
      fclose(datfile);
      segc++;
    }
    flags = nextflags;
  }
  fclose(tapefile);

  fprintf(stderr, "\n");
  return 0;
}


