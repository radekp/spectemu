#include "scrshot.h"
#include "z80.h"
#include "interf.h"
#include "misc.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

static void scrsh_scr_save(FILE *fp)
{
  fwrite(z80_proc.mem + 16384, 6144 + 768, 1, fp);
}

static void save_screenshot_file_type(char *name, int type)
{
  FILE *scrsh;

  scrsh = fopen(name, "wb");
  if(scrsh == NULL) {
    sprintf(msgbuf, "Could not open screenshot file `%s', %s",
            name, strerror(errno));
    put_msg(msgbuf);
    return;
  }

  if(type == SCRSHOT_SCR) scrsh_scr_save(scrsh);

  fclose(scrsh);
}

void save_screenshot_file(char *name)
{
  int type;

  strncpy(filenamebuf, name, MAXFILENAME-10);
  filenamebuf[MAXFILENAME-10] = '\0';

  type = SCRSHOT_SCR;
  if(check_ext(filenamebuf, "scr")) type = SCRSHOT_SCR;
  else {
    add_extension(filenamebuf, "scr");
    type = SCRSHOT_SCR;
  }

  save_screenshot_file_type(filenamebuf, type);

  sprintf(msgbuf, "Saved screenshot to file %s", filenamebuf);
  put_msg(msgbuf);
}

void save_screenshot(void)
{
  char *name;

  put_msg("Enter name of screenshot file to save:");

  name = spif_get_filename();
  if(name == NULL) return;

  save_screenshot_file(name);
}
