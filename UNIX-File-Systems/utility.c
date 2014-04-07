#include "utility.h"

void mount_root()
{
	char buf[1024];
	char rootdev[100];
	int bytes_read;
    int offset, block;
    MINODE root_minode;
    
    printf("Please Enter Rootdev: ");
    scanf("%s", &rootdev);
    fd = open(rootdev, O_RDWR);

    if(fd == -1)
    {
        printf("MOUNT_ROOT ERROR: Could Not Open Rootdev!\n");
        exit(1);
    }

    //Offset Boot block (first 1024 bytes) to get to start of super block
    if ((bytes_read = read(fd , buf, 1024)) != 1024)
    {
          printf("MOUNT_ROOT ERROR: Offset != 1024 bytes!\n");
          exit(1);
    }
    
    //Read super block into global struct super
    read(fd, &super, sizeof(SUPER));

    //Make sure super is EXT2 file_system type
    if(super.s_magic != 0xef53)
    {
          printf("MOUNT_ROOT ERROR: Super Block Not Type EXT2 FS!\n");
          exit(1);
    }

    //Get to the Group Desc Block and read into global struct gd
    lseek(fd, 1024+sizeof(SUPER), 0);
    read(fd, &gd, sizeof(GD));

	//Seek to block 2 to get root inode
	block = 2 / super.s_inodes_per_group;
    //Set the offset to inode size (128bytes)
    offset = sizeof(INODE);
	//printf("OFFSET: %d", offset);

    //Seek to root Inode Block + offset 1 inode, read into inode struct holder
    lseek(fd, (1024 + (gd.bg_inode_table -1)*1024) + offset, 0);
	read(fd, &inode, sizeof(INODE));

    //Fill in root MINODE information
    root_minode.INODE = inode;
    root_minode.dev = fd;
    root_minode.ino = 2;
    root_minode.refCount = 1;
    root_minode.dirty = 0;

    //Establish first minode entry as root
    minode[0] = root_minode;

    //Set Up global root PTR
    root = &minode[0];
    //Proc should be set to root
    proc[0].cwd = root;

    // increase reference count by one
    minode[0].refCount++;
    running = proc[0];

    running.cwd = root;

    printf("Mount %s  ", rootdev); printf("mounted on /\n");

    printf("Number blocks = %d\nBlocks free = %d\nNumber inodes = %d\nInodes free = %d\n", super.s_blocks_count, gd.bg_free_blocks_count, super.s_inodes_count, gd.bg_free_inodes_count);

    printf("Root Sucessfully Mounted! Hooray!\n");
}

void init()
{
	int i = 0;
    
    // set proc[0] values to super user
    proc[0].pid = 0;
    proc[0].uid = 0;
    proc[1].pid = 1;
    
    //proc cwds set to 0
    proc[0].cwd = 0;
    proc[1].cwd = 0;
    //all minodes refcount = 0
    while(i < 100)
    {
        minode[i].refCount = 0;
        i++;
    }
    //set root MINODE ptr to null
	root = NULL;
}

//get the directory name given from the path name
char* my_dirname(char* pathname, char* dir)
{
    char* temp, temppath[128];

    strcpy(temppath, pathname);
    temp = strtok(temppath, "/");
    strcat(dir, "/");
    strcat(dir, temp);
    while ((temp=strtok('\0', "/")) != '\0')
    {
		strcat(dir, "/");
		strcat(dir, temp);
    }   
    return dir;
}

// get the base name from the given path name.
char* my_basename(char* pathname, char* base)
{
	int i = 0;
	char temppath[128];
	char* temp, *prev;

	strcpy( temppath, pathname);
	prev = strtok(temppath, "/");
	for(i = 0; (temp = strtok('\0', "/") )!= '\0'; i++) //got to end of pathname
	{
	    strcpy(prev, temp);
	}

	strcat(base, prev); //cat last node value to basename
}

// break apart pathname;
int token_path(char* pathname)
{
     char* temp;
     char temppath[256];
     int n;

     strcpy(temppath, pathname);

     temp = strtok(temppath, "/");
     strcpy(name[0], temp);
     for (n = 1; (temp = strtok('\0', "/")) != '\0'; n++)
     {
          strcpy(name[n], temp);
     }

     return n;
}

unsigned long getino(int *dev, char* pathname)
{
     char *cp;
     char buff[BLOCK_SIZE] = {'\0'};
     char buf[BLOCK_SIZE] = {'\0'};
     int size = 0;
     int i = 0 ,flag = 0;
     int ino;
     int offset, block_num, inode_num;
     int strl;
     INODE *ip;

     get_block(fd, running.cwd->INODE.i_block[0], buff);

     // point cp and dp to buff
     cp = buff;
     dp = (DIR*)buff;

     //get strlen of the name field
     strl = strlen(name[i]);
     // while size of the specified inodes is less than current block size
     // printf("n is: %d\n", n);

     while(i < n)
     {
		size = 0; flag = 0;
        while (size < BLOCK_SIZE)
        {
		    //strncpy(
			//printf("Name[i] = %s\n", name[i]);
			//printf("dp->name = %s\n", dp->name);
			//printf("dp->name_len = %d\n", dp->name_len);
			//printf("strl = %d\n", strl);
					
            if ((strncmp(name[i], dp->name, dp->name_len) == 0) && (dp->name_len >= strlen(name[i])))
            {
				printf("%s Inode = %d\n", name[i], dp->inode);
                ino = dp->inode;
                flag = 1; //found
                break;
            }
            // move pointer to next dir entry
            cp += dp->rec_len;
			size += dp->rec_len;
			dp = ( DIR* )cp;
		}
            
        if(flag == 0)
        {
			return 0; // not found no need to go on
		}
		//Go to INODE
		block_num = (ino - 1)/INODES_PER_BLOCK + gd.bg_inode_table;
        inode_num = (ino - 1)%INODES_PER_BLOCK;
              
        //printf("Inode_num: %d\n", inode_num);
        get_block(fd, block_num, buf);
		//printf("Block_num: %d\n", block_num); 
		//Get the first direct block of INODE
        ip = (INODE *)buf + inode_num;
        get_block(fd, ip->i_block[0], buff);
        printf("Block Number = %d\n", ip->i_block[0]); 
        cp = buff;
        dp = (DIR*)buff;
        i++;
	}
    return ino;
}

unsigned long search(char *name)
{
     char *cp;
     int size = 0;
     int offset;
     char buff[1024];

     get_block(fd, running.cwd->INODE.i_block[0], buff);
     cp = buff;
     dp = (DIR*)buff;

     while (size < BLOCK_SIZE)
     {
		 if (strncmp(name, dp->name, dp->name_len) == 0)
		 {
			 return 0;
		 }
          cp += dp->rec_len;
          size += dp->rec_len;
          dp = (DIR*)cp;
     }
     return -1;
}

MINODE *iget(int dev, unsigned long ino)
{
    int i = 0, block_num, inode_num;
    INODE* ip;
    char buf[BLOCK_SIZE];
    while(minode[i].refCount != 0)
    {
        if(minode[i].ino == ino) //minode for this inode already exists
        {
            minode[i].refCount++; //increment ref count and return the minode
            //printf("Updated the RefCount @ MINDODE[%d] to %d\n", i, minode[i].refCount);
            return &minode[i];
        }

        i++;
    }

    i = 0;
    while(minode[i].refCount != 0) //Get to fresh minode
    {
        i++;
    }

	//Seek to the inode
    block_num = (ino - 1)/INODES_PER_BLOCK + gd.bg_inode_table;
    inode_num = (ino - 1)%INODES_PER_BLOCK;
    get_block(fd, block_num, buf);

    ip = (INODE *)buf + inode_num; //point to the inode

	//set up this minode
    minode[i].INODE = *ip;
    minode[i].refCount = 1;
    minode[i].ino = ino;
    minode[i].dev = dev;
    minode[i].dirty = 0;
    
    printf("iget Minode[%d]\n", i);
    return &minode[i]; //return new minode entry
}

void iput(MINODE *mip)
{
    int block_num, ino, offset, inode_num;
    char buf[BLOCK_SIZE], dbuf[1024*4];
    INODE *ip;

    mip->refCount--;

    if (mip->refCount > 0) //if still have refCount exit
    {
        return;
	}
    if (mip->dirty == 0) //if not dirty, no need to write to disk
    {
        return;
	}
    ino = mip->ino; //seek to ino location and get block
    block_num = (ino - 1)/INODES_PER_BLOCK + gd.bg_inode_table;
    inode_num = (ino - 1)%INODES_PER_BLOCK;
    get_block(mip->dev, block_num, buf);

    ip = (INODE *)buf + inode_num;
	*ip = mip->INODE; //update INODE ptr
	//write back to block
	put_block(mip->dev, block_num, buf);
	//printf("iput RefCount: , at Block: %d", mip->refCount, block_num);
    //printf("iput Block: %d\n", block_num);
}

int get_block(int dev, int blk, char *buf)
{
    lseek(dev,(long)BLOCK_SIZE*blk,0);
    read(dev, buf, BLOCK_SIZE);
}

int put_block(int dev, int blk, char *buf)
{
    lseek(dev, (long)BLOCK_SIZE*blk,0);
    write(dev, buf, BLOCK_SIZE);
}

unsigned long ialloc(int dev)
{
    int i;
    char buff[BLOCK_SIZE];            // BLKSIZE=block size in bytes
    int ninodes = super.s_free_inodes_count;
    // get inode Bitmap into buf[ ]


    get_block(fd, 4, buff);       // assume FD, imap block# = 4

    for (i=0; i < ninodes; i++){  // assume you know ninodes
        if (tst_bit(buff, i)==0){    // assume you have tst_bit() function
            set_bit(buff, i);          // assume you have set_bit() function
            put_block(fd, 4, buff);   // write imap block back to disk


            // update free inode count in SUPER and GD on dev
            super.s_free_inodes_count--;
            gd.bg_free_inodes_count--;
            return (i+1);
        }
    }

    return 0;                     // no more FREE inodes
}

void idealloc(int dev, unsigned long ino)
{
  int i;
  char buf[BLOCK_SIZE] = {'\0'};

  // get inode bitmap block
  get_block(dev, 4, buf);
  clearbit(buf, ino-1);         // assume you have clr_bit() function

  // write buf back
  put_block(dev, 4, buf);

  // update free inode count in SUPER and GD
  super.s_free_inodes_count++;
  gd.bg_free_inodes_count++;
}

void bdealloc(int dev, unsigned long block)
{
    int i;
    char buf[BLOCK_SIZE] = {'\0'};

    get_block(dev, 3, buf);

    clearbit(buf, block-1);

    put_block(dev, 3, buf);
    super.s_free_blocks_count++;
    gd.bg_free_blocks_count++;
}
unsigned long balloc(int dev)
{
    int i;
    char buf[BLOCK_SIZE];            // BLKSIZE=block size in bytes
    int nblocks = super.s_free_blocks_count;
    // get inode Bitmap into buf[ ]


    get_block(fd, 3, buf);       // assume FD, bmap block# = 3

    for (i=0; i < nblocks; i++){  // assume you know ninodes
        if (tst_bit(buf, i)==0){    // assume you have tst_bit() function
        set_bit(buf, i);          // assume you have set_bit() function
        put_block(fd, 3, buf);   // write imap block back to disk

        // update free inode count in SUPER and GD on dev
        super.s_free_blocks_count--;
        gd.bg_free_blocks_count--;
        return (i+1);
        }
    }
    return 0;                     // no more FREE inodes
}
// set bit_th bit in char buf[1024] to 1
int set_bit(char *buf, int bit)
{
    int i,j;
    i = bit / 8;
    j = bit % 8;
    buf[i] |= (1 << j);
    return 1;
}

int tst_bit(char* buf, int bit) //test a bit in buf
{
    int i,j;
    i = bit / 8;
    j = bit % 8;
    if ( buf[i] & (1 << j) )
        return 1;
}

int clearbit(char *buf, int bit) //clear a bit in buf
{
  int i,j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 1;
}

//Simple print menu
void menu()
{
    printf("\n----Commands Available:----\n");
    printf("mkdir	pathname/new directory  or  just new directory\n");
    printf("rmdir   pathname/directory  or  just directory\n");
    printf("ls      pathname\n");
    printf("pwd\n");
    printf("cd      pathname\n");
    printf("stat    file\n");
    printf("touch   file\n");
    printf("creat   file\n");
    printf("link    original_file   link_to_file\n");
    printf("unlink  file\n");
    printf("symlink original_file   link_to_file\n");
    printf("chmod   new_mode  file\n");
    printf("quit\n");
    printf("----------------------------\n");
}

int get_index(char* command)
{
    char command_list[20][20] = {"cd", "ls","pwd","mkdir", "creat", "rmdir", "link", "unlink", "symlink", "touch", "chmod", "stat", "menu", "quit"};
    int i = 0;

    while(i < 14)
    {
        if(strcmp(command, command_list[i]) == 0)
        {
            return i;
		}
        i++;
    }
    return -1;
}

void execute_cmd(int num, char* pathname)
{
    switch(num)
    {
        case 0: do_cd(pathname);
                break;
        case 1: do_ls(pathname);
                break;
        case 2: do_pwd();
                break;
        case 3: make_dir();
                break;
        case 4: creat_file();
                break;
		case 5: rmdir(pathname);
                break;
        case 6: link(pathname);
                break;
        case 7: unlink(pathname);
                break;
        case 8: symlink(pathname);
                break;
        case 9: touch(pathname);
                break;
        case 10: my_chmod(pathname);
                break;
        case 11: my_stat(pathname);
                break;
        case 12: menu();
                break;
        case 13: exit(1);
                break;
        default: printf("\n");
                break;
    }
}

//NOTE I DID NOT WRITE THIS FUNCTION BUT WAS GIVEN PERMISSION TO USE BY KCWANG 11/15/2012
//SOURCE:http://opensource.apple.com/source/Libc/Libc-166/string.subproj/strmode.c
//SIMPLY RECIEVES A FILES MODE AND RETURN A STRING OF ITS PERMISSIONS INTO CHAR *P
//MANUALLY INCLUDED BECAUSE DEV LIBRARIES <bsd/string.h> could not be recognized on my system
//USED FOR LS ONLY
void strmode(mode_t mode,char *p)
{
	 /* print type */
	switch (mode & S_IFMT) {
	case S_IFDIR:			/* directory */
		*p++ = 'd';
		break;
	case S_IFCHR:			/* character special */
		*p++ = 'c';
		break;
	case S_IFBLK:			/* block special */
		*p++ = 'b';
		break;
	case S_IFREG:			/* regular */
		*p++ = '-';
		break;
	case S_IFLNK:			/* symbolic link */
		*p++ = 'l';
		break;
	case S_IFSOCK:			/* socket */
		*p++ = 's';
		break;
#ifdef S_IFIFO
	case S_IFIFO:			/* fifo */
		*p++ = 'p';
		break;
#endif
#ifdef S_IFWHT
	case S_IFWHT:			/* whiteout */
		*p++ = 'w';
		break;
#endif
	default:			/* unknown */
		*p++ = '?';
		break;
	}
	/* usr */
	if (mode & S_IRUSR)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWUSR)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXUSR | S_ISUID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXUSR:
		*p++ = 'x';
		break;
	case S_ISUID:
		*p++ = 'S';
		break;
	case S_IXUSR | S_ISUID:
		*p++ = 's';
		break;
	}
	/* group */
	if (mode & S_IRGRP)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWGRP)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXGRP | S_ISGID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXGRP:
		*p++ = 'x';
		break;
	case S_ISGID:
		*p++ = 'S';
		break;
	case S_IXGRP | S_ISGID:
		*p++ = 's';
		break;
	}
	/* other */
	if (mode & S_IROTH)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWOTH)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXOTH | S_ISVTX)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXOTH:
		*p++ = 'x';
		break;
	case S_ISVTX:
		*p++ = 'T';
		break;
	case S_IXOTH | S_ISVTX:
		*p++ = 't';
		break;
	}
	*p++ = ' ';		/* will be a '+' if ACL's implemented */
	*p = '\0';
}
