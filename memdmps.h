#define MEMDMP_FAILED 0
#define MEMDMP_OK     1

int memdmp_write(unsigned char *, int, char *);
int memdmp_read(unsigned char *, int, char *);
int char_hexvalue(char);
int char2_hexvalue(char *);
int char4_hexvalue(char *);
int memdmp_editor(unsigned char *);
