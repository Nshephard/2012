#ifndef CMDS_H
#define CMDS_H

#include "type.h"
#include "utility.h"

//Prototypes for Commands
int do_cd(char* pathname);
int do_ls(char* pathname);
void do_pwd();
int pwd(MINODE* wd);
int make_dir();
int my_mkdir(MINODE *pip, char *name);
void creat_file();
int my_creat(MINODE *pip, char *name);
int rmdir(char* pathname);
int rm_child(MINODE* pip, char* my_name);
int link(char* pathname);
int unlink(char* pathname);
int symlink(char* pathname);
int touch(char* pathname);
int my_chmod(char* pathname);
void my_stat(char* pathname);
int do_stat(char* pathname, struct stat *stPtr);
#endif
