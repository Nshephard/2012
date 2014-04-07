//#include "type.h"
#include "cmds.h"

void mount_root();
void init();
int token_path(char pathname[256]);
char* my_dirname(char* pathname, char* dir);
char* my_basename(char* pathname, char* base);
unsigned long getino(int *dev, char* pathname);
unsigned long search(char *name);
MINODE *iget(int dev, unsigned long ino);
void iput(MINODE *mip);
int get_block(int dev, int blk, char *buf);
int put_block(int dev, int blk, char *buf);
unsigned long ialloc(int dev);
unsigned long balloc(int dev);
void idealloc(int dev, unsigned long ino);
void bdealloc(int dev, unsigned long block);
int set_bit(char *buf, int bit);
int tst_bit(char* buf, int bit);
int clearbit(char *buf, int bit);
void menu();
int get_index(char* command);
void execute_command(int num, char* path);
void strmode(mode_t mode,char *p);
