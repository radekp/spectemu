#include "tapefile.h"
#include "tapef_p.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char *tapename;
  byte *head;
  int i;

  tapename = argv[1];
  
  if(!open_tapefile(tapename, TAP_TZX)) {
    fprintf(stderr, "%s\n", seg_desc);
    return 1;
  }

  printf("TZX Version %i.%02i\n", tf_tpi.tzxmajver, tf_tpi.tzxminver);
  
  for(i = 0; (head = tf_get_block(i)) != NULL; i++) {
    if(head != NULL) printf("%4i (%05lX): %02X l:%5li p:%5i n:%5i u:%i %s\n", 
			    i, tf_segoffs, head[0], 
			    tf_cseg.len, tf_cseg.pause, 
			    tf_cseg.num, tf_cseg.bused, seg_desc);
  } while(head != NULL);

  if(head == NULL) printf("%s\n", seg_desc);

  return 0;
}

