#include <stdio.h>

int main(int argc, char *argv[])
{
  FILE *in = stdin;
  FILE *out = stdout;

  double av = 0.0;
  double ex = 0.33;
  double ex_i = 1.0 - ex;

  double avu = 0.0;
  double exu = 0.50;
  double exu_i = 1.0 - exu;
    

  int c;
  double v;

  while((c = getc(in)) != EOF) {
    v = (double) c - 128.0;
    avu = (avu * exu_i) + (v * exu);
    av = (av * ex_i) + (v * ex);
    if(argc < 2) {
      if(avu - av < 0) putc(64, out);
      else putc(-64, out);
    }
    else {
      putc((int) ((avu - av) * 5.0) + 128, out);
    }
  }

  return 0;
}

