#include "cmds.h"

int do_cd(char* pathname)
{
    int ino;
    MINODE *mip;
    char temp[256];

	//if path not entered or / cd to root
    if (pathname[0] == 0 || strcmp(pathname , "/") == 0)
    {
        iput(running.cwd);
        running.cwd = root;
        root->refCount++;
        ino = root->ino;
    }

    else
    {
        strcpy(temp, pathname);
        n = token_path(temp);
        printf("Tokens = %d\n", n);
        ino = getino(&fd, running.cwd);
        
        if (ino == 0) //if inode == 0 then not found
        {
			printf("CD ERROR: File INODE not found!\n");
            return 0;
		}

        mip = iget(fd, ino); //put the inode into MINODEs

        //check to see if directory
        if (S_ISREG(mip->INODE.i_mode))
        {
            printf("CD ERROR: Not a Directory!\n");
            iput(mip); //YELL AT USER; return BAD
            return -1;
        }
        running.cwd = mip; //update running to point at MINODE
    }
    return 0;
}

int do_ls(char* pathname)
{
    time_t time = (time_t)inode.i_ctime; //current time
    char time_buf[128], permissions[128], namebuf[256]; //variable buffers
    char buf[BLOCK_SIZE] = {'\0'}; //block buffer
    int block_num, size = 0, offset, ino;
    MINODE *mip;
    char *cp;
    

    if(pathname[0] == 0) //if no pathname use cwd, read block into buf
    {
        get_block(fd, running.cwd->INODE.i_block[0], buf);
    }
    else //parse to the pathname
    {
        n = token_path(pathname);
        ino = getino(&fd, pathname);
        mip = iget(fd,ino);
        get_block(fd, mip->INODE.i_block[0], buf);
    }

    cp = buf;
    dp = (DIR*)buf;

    while (size < BLOCK_SIZE)
    {
	    strncpy(namebuf, dp->name, dp->name_len); //get name
	    namebuf[dp->name_len] = '\0';
        time = (time_t)inode.i_mtime; //get time
        strmode(inode.i_mode, permissions); //get permissions
        strcpy(time_buf,ctime(&time)); //convert to ctime and into time var
        //get rid of newline ctime adds
        time_buf[strlen(time_buf)-1]='\0';
        //print stats
        printf("%s  %d  %d  %d  %s  %d  %s\n", permissions, inode.i_links_count, inode.i_uid, inode.i_gid, time_buf, inode.i_size, namebuf);
        cp += dp->rec_len; //increment to next dir enttry
        size += dp->rec_len;
        dp = (DIR*)cp; 
        block_num = dp->inode / super.s_inodes_per_group; //increment to next inode's block
        offset = dp->inode - (block_num * super.s_inodes_per_group) -1;
        lseek(fd, (1024 + (gd.bg_inode_table - 1) * 1024) + offset * sizeof(INODE), 0);
        read(fd, &inode, sizeof(INODE));
	}    
	inode = running.cwd->INODE; //set global inode as the cwd
}

//pwd function harness
void do_pwd()
{
    pwd(running.cwd);
    putchar('\n');//append newline
}

//PWD - recursively get parent+ino
int pwd(MINODE* wd)
{
    char buf[BLOCK_SIZE];
    int inumber, size = 0;
    char *cp;
    INODE *ip;
    char namebuf[128];
    int parent_ino;
    
    //if root simply print / and return
    if(wd == &minode[0]) 
    {
        printf("Pwd: /");
        return 0;
    }
	//get first direct block into buff
    get_block(fd, wd->INODE.i_block[0], buf);
	//set parent to wd ino
    parent_ino = wd->ino;
    cp = buf; //adjust char and dir ptrs
    dp = (DIR*)buf;

    cp += dp->rec_len;//increment to next dir (parent ..)
    dp = (DIR*)cp;

    wd = iget(fd, dp->inode); //get parent's inode
	pwd(wd); //recursively call pwd on the new wd

    get_block(fd, wd->INODE.i_block[0], buf);//Now at root
    cp = buf;
    dp = (DIR*)buf;
    while(size < BLOCK_SIZE)
    {
		if(dp->inode == parent_ino) //if current dir inode = parent's
        {
			if(strncmp(dp->name, ".", dp->name_len) != 0 && strncmp(dp->name, "..", dp->name_len) != 0)
			{
				if (dp->inode == running.cwd->ino)  //print parent dir and recursively return
				{
                strncpy(namebuf, dp->name, dp->name_len); //if in cwd dont append /
				namebuf[dp->name_len] = '\0';
                printf("%s", namebuf);
				}
				else
				{
                strncpy(namebuf, dp->name, dp->name_len); //if in a parent append /
				namebuf[dp->name_len] = '\0';
                printf("%s/", namebuf);					
				}
			}
                return 0;
         } //increment to next dir entry
         cp += dp->rec_len;
         size += dp->rec_len;
         dp = (DIR*)cp;
	}
}

int make_dir()
{
    char parent[128] = {'\0'}, child[128] = {'\0'}; //hold parent/child paths
    int parent_ino, r;
    MINODE *pip;
    MINODE *cwd_temp = running.cwd; //hold cwd since it is clobbered to find parent
    
    if(pathname[0] == 0)
    {
        printf("MKDIR ERROR: No Input Path Specified!\n");
        return;
    }
    if(pathname[0] == '/') //if root
    {
        fd = root->dev;
        running.cwd = &minode[0];
    }
    
    my_dirname(pathname, parent); //Break up pathname into dir and base (parent and child)
    my_basename(pathname, child);
    printf("PARENT (Dirname): %s\n", parent); //print tracing of parent dir and base
    printf("CHILD (Basename): %s\n", child);

    n = token_path(parent); //parse the dir to get the ino of parent

    if(n == 1) //if token_path return 1, then use cwd as parent
    {
        parent_ino = running.cwd->ino;
    }
    else  //get parent's ino
    {
        n--;
        parent_ino = getino(&fd, parent);
        
        if(parent_ino == 0) //if bad parent ino exit
        {
            printf("MAKE_DIR ERROR: Invalid Path!\n");
            return;
        }
    }

    running.cwd = cwd_temp; //restore running to original cwd
    pip = iget(fd , parent_ino);
    
    printf("Parent INODE (pip->ino) = %d\n", pip->ino); //print to confirm pip correct inode

	if (!S_ISDIR(pip->INODE.i_mode)) //make sure parent is DIR
	{
		printf("MAKE_DIR ERROR: Parent is not DIR!\n");
		return -1;
	}

	/*
    r = getino(&fd, pathname);
    printf("R PATHNAME: %d", r);
    
	if (r != 0) //the ino was already found for child
	{
		printf("MAKE_DIR ERROR: Child already exists!\n");
		return -1;
	} */
	r = 0;
    r = my_mkdir(pip, child);
    if (r >= 0)
    {
		printf("MAKEDIR SUCESSFUL HOORAY!\n");
	}
	else
	{
		printf("MAKEDIR ERROR: my_mkdir return error!\n");
	}
	return r;
}

int my_mkdir(MINODE *pip, char *name)
{
    unsigned long inumber, bnumber;
    int i = 0, name_len, need_length, ideal_length, size = 0;
    char buf[BLOCK_SIZE] = {'\0'}, *cp;
    char block_buf[BLOCK_SIZE] = {'\0'};
    MINODE *mip;
    int ino, offset;
    pathname[0] = 0;
    
	//Allocate an INODE and a BLOCK
    inumber = ialloc(fd);
    bnumber = balloc(fd);

    mip = iget(fd, inumber); //add new inode to MINODES

    mip->INODE.i_mode = 0x41ED;		/* DIR and permissions */
    mip->INODE.i_uid  = running.uid;	/* Owner Uid */
    mip->INODE.i_gid =  running.gid;	/* Group Id */
    mip->INODE.i_size = 1024 ;		/* Size in bytes */

    mip->INODE.i_links_count = 2;	/* Links count */

    mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L);

    mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
    mip->dirty = 1;               /* mark dirty */

	//set direct blocks for dir to 0
    for (i=0; i<15; i++)
    {
        mip->INODE.i_block[i] = 0;
	}

    mip->INODE.i_block[0] = bnumber; //set its direct block to block# allocated

	printf("INODE.i_block[0] = %d\n", mip->INODE.i_block[0]);
	
    iput(mip); //put it in MINODES

    dp = (DIR *)buf; //Write the . and .. entires into buf

    dp->inode = inumber;		/* Inode number */
    strncpy(dp->name, ".", 1);    /* File name */
    dp->name_len = 1;		/* Name length */
    dp->rec_len = 12;		/* Directory entry length */

    cp = buf;
    cp += dp->rec_len;            /* advance by rec_len */
    dp = (DIR *)cp;

    dp->inode = pip->ino;      /* Inode number of parent DIR */
    dp->name_len = 2;		/* Name length */
    strncpy(dp->name, "..", 2);   /* File name */
    dp->rec_len = BLOCK_SIZE - 12;/* last DIR entry length to end of block */



    put_block(fd, bnumber, buf);
    printf("PutBlock updated block: %d\n", bnumber);
    
    ino = pip->ino; //Add to Parent's Directory
    //Seek to Parent's Inode
    offset = ino -1;
    lseek(fd, (1024 + (gd.bg_inode_table - 1) * 1024) + offset * sizeof(struct ext2_inode), 0);
    read(fd, &inode, sizeof(struct ext2_inode));

	//Seek to Parent's 1st Direct Block
    offset = 1024 + (inode.i_block[0] - 1) *1024;
    lseek(fd, offset, 0);
    read(fd, block_buf, 1024);

    cp = block_buf; //Setup DIR PTRS
    dp = (struct ext2_dir_entry_2*)buf;
    name_len = strlen(name);
    need_length = 4*((8 +name_len+3)/4); //Get Needed_Length in block

    while (size < BLOCK_SIZE) //Seek to the last entry in the Data Block
    {
        cp += dp->rec_len;
        size += dp->rec_len;
        //dp_prev = dp;
        dp = (struct ext2_dir_entry_2*)cp;
        if (dp->rec_len + size == BLOCK_SIZE) //You have found last dir entry
        {
            break;
		}
    }
    ideal_length = 4*((8+dp->name_len+3)/4); //calculate ideal length for last directory

    if(dp->rec_len - ideal_length >= need_length) //If disk has room for needed length
    {											  //Based on rec_len remaining - ideal_length
        dp->rec_len = ideal_length; //Creat a slot of ideal_length

        cp += dp->rec_len;
        size += dp->rec_len;
        dp = (struct ext2_dir_entry_2*)cp;

        dp->rec_len = BLOCK_SIZE - size; //update new dir rec_len to end of disk
        strncpy(dp->name, name , name_len); 
        dp->name_len = name_len; //update new dir name_len
        dp->inode = mip->ino; //update new dir's inode
        dp->file_type = 2; //set to file_type 2 (DIR)
    }

    put_block(fd, inode.i_block[0], block_buf); //re-insert updated block
    printf("Mkdir PutBlock into block: %d\n", inode.i_block[0]);

    pip->INODE.i_links_count++; //increment parent's link count + update time + dirty
    pip->INODE.i_atime=pip->INODE.i_ctime=pip->INODE.i_mtime = time(0L);
    pip->dirty = 1;
    iput(pip); //Put updated parent into MINODES
    return 0;
}

void creat_file()
{
    char dir[128] = {'\0'}, base[128] = {'\0'};
    int ino, r;
    MINODE *pip;
    MINODE *cwd_temp = running.cwd;
    if(pathname[0] == 0)
    {
        printf("CREAT ERROR: No Input Path Specified\n");
        return;
    }
    if(pathname[0] == '/') //if root
    {
        fd = root->dev;
        running.cwd = &minode[0];
    }

    my_dirname(pathname, dir);
    my_basename(pathname, base);

    n = token_path(dir);
	printf("CREAT N = %d\n", n); //number of tokens

    if(n == 1)
    {
        ino = running.cwd->ino;
    }
    //if( running.cwd->ino != root.ino )
    else
    {
        n--; //subtract 1 token to get the ino of the parent
        ino = getino(&fd, dir);
        if(ino == 0) //make sure parent exists
        {
            printf("CREAT ERROR: Invalid Path\n");
            return 0;
        }
    }

    running.cwd = cwd_temp; //hold cwd in temp

    pip = iget(fd , ino); //acess the parent's MINODE
    printf("Ino Num: %d\n", ino);

    r = my_creat(pip, base); //creat the new file based pass in parent's inode
}
//Same as my_mkdir but no block is allocated, just an inode
int my_creat(MINODE *pip, char *name)
{
    unsigned long inumber, bnumber;
    int i = 0, name_len, need_length, ideal_length, size = 0;
    char buf[BLOCK_SIZE] = {'\0'}, *cp;
    char buff[BLOCK_SIZE] = {'\0'};
    MINODE *mip;
    int ino, block_num, offset;
    DIR *dp_prev;

    inumber = ialloc(fd);

    mip = iget(fd, inumber); //Get new INODE into MINODES

	//Set up INODE to be FILE_MODE /w 0 size
    mip->INODE.i_mode = FILE_MODE;		/* DIR and permissions */
    mip->INODE.i_uid  = running.uid;	/* Owner Uid */
    mip->INODE.i_gid =  running.gid;	/* Group Id */
    mip->INODE.i_size = 0 ;		/* Size in bytes */

    mip->INODE.i_links_count = 1;	/* Links count */

    mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L);

    mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
    mip->dirty = 1;               /* mark dirty */

    for (i=0; i<15; i++) //Clear out its direct blocks
    {
        mip->INODE.i_block[i] = 0;
	}

    iput(mip); //update via MINODES

    ino = pip->ino; //seek to parent's inode
    block_num = ino / super.s_inodes_per_group;
    offset = ino - (block_num * super.s_inodes_per_group) -1;
    lseek(fd, (1024 + (gd.bg_inode_table - 1) * 1024) + offset * sizeof(struct ext2_inode), 0);
    read(fd, &inode, sizeof( struct ext2_inode)); //read in parent's inode

    get_block(fd,inode.i_block[0], buff); //gets it first block and start going through dirs
    cp = buff;
    dp = (struct ext2_dir_entry_2* )buff;
    name_len = strlen(name);

    need_length = 4*((8 + name_len + 3)/4); //get your needed length

    while (size < inode.i_size) //iterate through directories until we get to the end
    {
        cp += dp->rec_len;
        size += dp->rec_len;
        //dp_prev = dp;
        dp = (struct ext2_dir_entry_2*)cp;
        if (dp->rec_len + size == BLOCK_SIZE)
        {
            break;
		}
    }
    ideal_length = 4*((8+dp->name_len+3)/4); //calculate ideal length of previous last entry

    if(dp->rec_len - ideal_length >= need_length) //if we have room for new entry add it!
    {
        dp->rec_len = ideal_length; //reallocate last block to be its ideal

        cp += dp->rec_len; //go to next dir and put in new item
        size += dp->rec_len;
        dp = (struct ext2_dir_entry_2*)cp;

        dp->rec_len = BLOCK_SIZE - size; //new rec_len is remainder of block
        strcpy(dp->name, name); //update its name/inode/type
        dp->name_len = name_len;
        dp->inode = inumber;
        dp->file_type = 1;
    }

    put_block(fd, inode.i_block[0], buff); //Update parent's block + mark dirty
	pip->dirty = 1;
    
    iput(pip); //Update Parent in MINODES
}


int rmdir(char* pathname)
{
    int ino;
    int i = 0; 
    int size = 0;
    char *cp;
    char buf[BLOCK_SIZE] = {'\0'};
    MINODE *mip, *pip;
    char base[64] = {'\0'};
    
    
    if(pathname[0] == 0)
    {
        printf("RMDIR ERROR: No Path Specified!\n");
        return -1;
    }
	//Get the Base
    my_basename(pathname, base);
    printf("BASENAME: %s\n", base);
    //Get the path and inode to DIR
    n = token_path(pathname);
    ino = getino(&fd, pathname);
    
    //If dir doesnt exist (0 inode) return error
    if(ino == 0)
    {
			printf("RMDIR ERROR: File Does Not Exist!\n");
			return -1;
	}

	printf("RMDIR Pathname: %s\n", pathname);
    mip = iget(fd, ino); //get a MINODE
    if(running.cwd->ino == ino) //make sure isn't currently in use
    {
       printf("RMDIR ERROR: Directory is Busy!\n");
       return -1;
    }
    
    if(!(S_ISDIR(mip->INODE.i_mode))) //make sure file read is a Directory
    {
        printf("RMDIR ERROR: File is Not a Directory!\n");
        return -1;
    }

	//Get the block for the inode into buf and point @ it
    get_block(fd, mip->INODE.i_block[0], buf);
    cp = buf;
    dp = (DIR*)buf;

    while(size < BLOCK_SIZE)
    {
        if(i >= 2) //if iteration through directories exceeds . and .. cannot remove
        {
            printf("RMDIR ERROR: Directory is Not Empty!\n");
            return -1;
        }
        cp += dp->rec_len;
        size += dp->rec_len;
        dp = (struct ext2_dir_entry_2*)cp;
        i++;
    }
	
    i = 0;
    for (i=0; i<12; i++)
    {
         if (mip->INODE.i_block[i]==0) //if i_block[i] is already 0 skip the deallocation of the block
         {
             continue;
		 }
         bdealloc(mip->dev, mip->INODE.i_block[i]); //deallocate direct blocks of DIR
    }
    //printf("GOT HERE1\n");
    idealloc(mip->dev, mip->ino); //deallocate minode
    iput(mip); //this should make the refcount 0, and write back to disk

    if(n == 1)//if within current directory 
    {
        ino = running.cwd->ino;
    }
    else //traverse through path (via the inodes) to base DIR
    {
        n--;
        ino = getino(&fd, pathname);
    }
	//get parent's inode
    pip = iget(mip->dev, ino); 
	//remove parent's child
    rm_child(pip, base);

	//Decrement parent's link count + update access/modified time
    pip->INODE.i_links_count--;
    pip->INODE.i_atime=pip->INODE.i_mtime = time(0L);
    pip->dirty = 1; //mark dirty update MINODE
    iput(pip);

    return 1; // success
}

//Updates parent's data block
int rm_child(MINODE* parent, char* my_name)
{
    char buf[BLOCK_SIZE] = {'\0'};
    char *cp;
    int size = 0;
    int new_rec;
    DIR* current_dir;
    
    //Get Parent's Block into buf
    get_block(parent->dev, parent->INODE.i_block[0], buf);

	//set up char and dir ptr
    cp = buf;
    dp = (DIR*)buf;


    while(size < BLOCK_SIZE)
    {
		current_dir = dp; //Hold parent's directory info
        cp += dp->rec_len; //increment to next DIR entry and current offset on block
        size += dp->rec_len;
        dp = (DIR*)cp;
        if(strcmp(my_name, dp->name) == 0) //If first entry in block
            {
				strcpy(dp->name, " "); //Clear Name
                new_rec = dp->rec_len; //Update Stored Rec_len
                if(size + dp->rec_len == BLOCK_SIZE)//If only entry
                {
                    dp = current_dir; //return to parent
                    dp->rec_len += new_rec; //adjust rec_length
                    break;
                }

                dp->name_len = 0; //Clear out remaining DIR fields
                dp->file_type = 0; 
                dp->inode = 0;

                while(size < BLOCK_SIZE) //Repeat until done!
                {
                    current_dir = dp; //increment current parent dir
                    cp += dp->rec_len;
                    size += dp->rec_len;
                    dp = (DIR*)cp;
                    *current_dir = *dp;
                     if(size + dp->rec_len == BLOCK_SIZE - new_rec) //update rec_len from last entry of block
                     {
                         dp->rec_len += new_rec;
                         break;
                     }

                     if(size + current_dir->rec_len == BLOCK_SIZE)//check if last entry
                     {
                         current_dir->rec_len += new_rec; //update last item new rec_len
                         break;
                     }
                }
                break;
            }
        }
     //Update Parent's Block
    put_block(parent->dev, parent->INODE.i_block[0], buf);
}

int link(char* pathname)
{
    char oldfile[64];
    char newfile[256] = {'\0'};
    char dir[64] = {'\0'}, base[64] = {'\0'};
    int flag;
    int block_num;
    int offset;
    MINODE *pip, *mip_old, *mip_new;
    int ino, size = 0, name_len = 0;
    int parent_ino;
    INODE *ip;
    int need_length, ideal_length;
    char buf[BLOCK_SIZE] = {'\0'};
    char *cp;
    
    if(pathname[0] == 0)
    {
        printf("LINK ERROR: No Input Path Specified!\n");
        return -1;
    }

    strcpy(oldfile, strtok(pathname, " ")); //get paths from source and new file
    //strcat(oldile, '\0')
    //n = token_path(oldfile);
    printf("oldfile path: %s\n", oldfile);
    //strcat(newfile, '\0');
    strcat(newfile, strtok('\0', " "));
	printf("newfile path: %s\n", newfile);

	n = token_path(oldfile);
	ino = getino(&fd, oldfile);
	printf("INO: %d\n", ino);
	//ino = getino(&fd, oldfile);
	//printf("INO: %d\n", ino);
	
	if(ino == 0) //make sure source exists
	{
		printf("LINK ERROR: Sourcefile Not Found!\n");
		return -1;
	}

    //reference the source inode
    mip_old = iget(fd, ino);
    
    if (!S_ISREG(mip_old->INODE.i_mode)) //make sure source is REG file
    {
		printf("LINK ERROR: Source is not a Regular File!\n");
		return -1;
	}

    my_dirname(newfile, dir);    
    my_basename(newfile, base);
    n = token_path(dir);
	

	//printf("BASE: %s\n", base);

	flag = getino(&fd, newfile); //get new ino, make sure doesn't already exist
	//printf("FLAG: %d\n", flag);
	printf("FLAG INO: %d\n", flag);
	if(flag != 0) //if ino exist then file already exists
	{
		printf("LINK ERROR: Newfile Already Exists!\n");
		return -1;
	}
	
	n = token_path(newfile); //Parse to base (newfile name) then -1 to get to parent
	if (n == 1)
	{
		parent_ino = running.cwd->ino;
	}
	else
	{
	n--;
    parent_ino = getino(&fd, dir); //get parent's inode to add new file
	printf("PARENT INO: %d\n", parent_ino); //get parent's ino
	}
    mip_new = iget(&fd, newfile); //get a new inod
    
    mip_new->INODE.i_mode = mip_old->INODE.i_mode; //copy stats from parent ino
    mip_new->INODE.i_uid= mip_old->INODE.i_uid;
    mip_new->INODE.i_gid = mip_old->INODE.i_gid;
	mip_new->INODE.i_size = mip_old->INODE.i_size;
	mip_new->INODE.i_atime=mip_new->INODE.i_ctime=mip_new->INODE.i_mtime = time(0L);
	mip_old->INODE.i_atime=mip_new->INODE.i_atime; //old_mip's access/modified time updated
	mip_old->INODE.i_mtime=mip_new->INODE.i_mtime; 

	//GEt to parent's INODE
    block_num = parent_ino / super.s_inodes_per_group;
    offset = parent_ino - (block_num * super.s_inodes_per_group) -1;
    lseek(fd, (1024 + (gd.bg_inode_table - 1) * 1024) + offset * sizeof(struct ext2_inode), 0);
    read(fd, &inode, sizeof(struct ext2_inode));

    get_block(fd,inode.i_block[0], buf); //read parent's first direct block
    cp = buf;
    dp = (struct ext2_dir_entry_2* )buf;
    name_len = strlen(name);

    need_length = 4*((8 + name_len + 3)/4); //figure out name_len

    while (size < inode.i_size) //increment to last entry
    {
        cp += dp->rec_len;
        size += dp->rec_len;
        //dp_prev = dp;
        dp = (struct ext2_dir_entry_2* )cp;
        if (dp->rec_len + size == BLOCK_SIZE)
        {
            break;
		}
    }
    ideal_length = 4*((8+dp->name_len+3)/4); //get ideal length of previous last entry

    if(dp->rec_len - ideal_length >= need_length) //see if you have room
    {
        dp->rec_len = ideal_length; //if you have room readjust previous entry to its ideal

        cp += dp->rec_len;
        size += dp->rec_len;
        dp = (struct ext2_dir_entry_2*)cp;

        dp->rec_len = BLOCK_SIZE - size; //update new item to get the rec->len to end
        dp->name_len = strlen(base);
        strcpy(dp->name, base);
        dp->inode = ino; //original file's inode
        dp->file_type = 2;
    }

    put_block(fd, inode.i_block[0], buf); //update parent's block
    
	mip_new->dirty = 1;
	mip_old->INODE.i_links_count++; //update links
    mip_new->INODE.i_links_count++; 
    iput(mip_new); //update via MINODES

    printf("Link Sucessful! HOORAY! Sourcefile: %s   New Link: %s\n", oldfile, newfile);
}

int unlink(char* pathname)
{
    char child[64] = {'\0'};
    int ino;
    MINODE *mip, *pip;


    n = token_path(pathname);
    ino = getino(&fd, pathname);
    mip = iget(fd, ino);

    if(!S_ISREG(mip->INODE.i_mode)) //linking must be a file
    {
        printf("UNLINK ERROR: Not a File!\n");
        return 0;
    }

    //mip->INODE.i_links_count--;
    my_basename(pathname, child);
    if(mip->INODE.i_links_count == 0) //if the link count is 0 must deallocate inode and block
    {
        idealloc(fd,ino);
        bdealloc(fd,mip->INODE.i_block[0]);
    }

    if(n == 1) //get the inode of the parent
    {
        ino = running.cwd->ino;
    }
    else
    {
        n--;
        ino = getino(&fd, pathname);
    }
    pip = iget(fd, ino); //put removed inode into rm_hild
    rm_child(pip, child ); //removes child directory
    mip->INODE.i_links_count--; //decrement parent's link count
    mip->dirty = 1; //mark dirty
    iput(mip); //UPDATE via MINODES
}


int symlink(char* pathname)
{
    char oldfile[60];
    char newfile[256];
    char dir[64] = {'\0'};
    char base[64] = {'\0'};
    MINODE *mip_old;
    MINODE *mip_new;
    int ino;
    int new_ino;


    char buf[BLOCK_SIZE] = {'\0'};

    strcpy(oldfile, strtok(pathname, " "));
    strcpy(newfile, strtok('\0', " "));

    n = token_path(oldfile);
    ino = getino(&fd, oldfile);
    if(n == 1)
    {
        ino = running.cwd->ino;
    }
    else
    {
        n--;
        ino = getino(&fd, newfile);
    }
    if(ino == 0)
    {
        printf("SYMLINK ERROR: Source File Does Not Exist!\n");
        return -1;
    }
    mip_old = iget(fd, ino);

    my_creatsym(mip_old, newfile, oldfile);
	
    my_dirname(newfile, dir);
    printf("oldNAME: %s\n", oldfile);
    printf("Parent INO: %d\n", ino);
    n = token_path(newfile); 
	//mip_new = iget(fd, new_ino);
	//mip_new->INODE.i_mode = S_IFLNK;
	//strcpy(mip_new->INODE.i_block[0], oldfile);
	//iput(mip_new);
    //mip_new = iget(fd, new_ino);
    new_ino = getino(&fd, dir);
    printf("NEW INO: %d\n", new_ino);
    mip_new = iget(fd, new_ino);
    get_block(fd, mip_new->INODE.i_block[0], buf);
    strcpy(buf, oldfile);
    put_block(fd, mip_new->INODE.i_block[0], buf);
    mip_new->INODE.i_size = strlen(oldfile);
    mip_new->INODE.i_mode = S_IFLNK;
    iput(mip_new);
}

//touch function (simply update the atime and mtime fields of the inode at path)
int touch(char* pathname)
{
    int ino;
    MINODE *mip;

	//parse the path get the ino
    n = token_path(pathname);
    ino = getino(&fd, pathname);
    
    //file exists check
    if(ino == 0)
    {
        printf("TOUCH ERROR: File Does Not Exist!\n");
        return -1;
    }

	//access MINODE, update atime and mtime, mark dirty and return to MINODEs
    mip = iget(fd, ino);
    mip->INODE.i_atime=time(0L);
    mip->INODE.i_mtime=time(0L);
    mip->dirty = 1;
    iput(mip);
}

int my_chmod(char* pathname)
{
    char permissions[64] = {'\0'}; //Permissions input
    char file[128] = {'\0'}; //file input
    char file_perm[4] = {"010"};
    char  dir_perm[4] = {"004"};
    int ino;
    long set_perm;
    MINODE *mip;

	//Get user input into new permissions and file
    strcpy(permissions, strtok(pathname, " "));
    strcpy(file, strtok('\0', " "));
    
    if(strlen(permissions) != 4) //Need size 4 permissions to update (to modify bytes of group not type)
    {
        printf("CHMOD ERROR: Invalid Permissions!\n");
        return -1;
    }
    n = token_path(file);
    ino = getino(&fd, file); //get the inode of the file
    mip = iget(fd, ino); //reference MINODES
    if(S_ISREG(mip->INODE.i_mode)) //Update if REG file
    {
        strcat(file_perm, permissions); //add new permissions to file
        set_perm = strtol(file_perm, '\0', 0); //convert string to long int
    }
    if(S_ISDIR(mip->INODE.i_mode))
    {
        strcat(dir_perm, permissions); //add new permissions to dir
        set_perm = strtol(dir_perm, '\0', 0); //convert string to long int
    }

    mip->INODE.i_mode = set_perm; //update the mode of the inode
    mip->dirty = 1; //mark dirty
    iput(mip); //update MINODEs
}

//my stat (driver to do_stat)
void my_stat(char* pathname)
{
    struct stat temp_stat;
    int r;

    r = do_stat(pathname, &temp_stat); //hold stat data in temp stat
	if (r != 0)
	{
		printf("STAT ERROR: do_stat Returned Error, File Does Not Exist!\n");
	}
}

//do_stat does the work of stat
int do_stat(char* pathname, struct stat *stPtr)
{
    int ino;
    MINODE *mip;

    n = token_path(pathname);

	//get inode of file
    ino = getino(&fd, pathname); 
    if(ino == 0) //if file doesn't exist return error to my_stat
    {
		return -1;
	}
    mip = iget(fd, ino); //load it's minode

	//Copy all entries of INODE into stat struct 
    stPtr->st_dev = fd;
    stPtr->st_ino = ino;
    stPtr->st_mode = mip->INODE.i_mode;
    stPtr->st_uid = mip->INODE.i_uid;
    stPtr->st_size = mip->INODE.i_size;
    stPtr->st_atime = mip->INODE.i_atime;
    stPtr->st_ctime = mip->INODE.i_ctime;
    stPtr->st_mtime = mip->INODE.i_mtime;
    stPtr->st_gid = mip->INODE.i_gid;
    stPtr->st_nlink = mip->INODE.i_links_count;
    stPtr->st_blocks = mip->INODE.i_blocks;
    stPtr->st_blksize = 1024;

    //print stat structure
    printf("------------------------------\n");
    printf("Stat path: %s\n", pathname);
    printf("Dev: %d\n", stPtr->st_dev);
    printf("Ino: %d\n", stPtr->st_ino);
    printf("Mode: %d\n", stPtr->st_mode);
    printf("Uid: %d\n", stPtr->st_uid);
    printf("Size: %d\n", stPtr->st_size);
    printf("Atime: %s", ctime(&stPtr->st_atime));
    printf("Ctime: %s", ctime(&stPtr->st_ctime));
    printf("Mtime: %s", ctime(&stPtr->st_mtime));
    printf("Gid: %d\n", stPtr->st_gid);
    printf("Nlink: %d\n", stPtr->st_nlink);
    printf("Blocks: %d\n", stPtr->st_blocks);
    printf("Blocksize: %d\n\n", stPtr->st_blksize);
	printf("------------------------------\n");

    iput(mip); //dereference used minode after printing
	return 0; //return 0 for sucess
}

int my_creatsym(MINODE *pip, char *name, char *link)
{
    unsigned long inumber, bnumber;
    int i = 0, name_len, need_length, ideal_length, size = 0;
    char buf[BLOCK_SIZE] = {'\0'}, *cp;
    char buff[BLOCK_SIZE] = {'\0'};
    MINODE *mip;
    int ino, block_num, offset;
    DIR *dp_prev;

    inumber = ialloc(fd);

    mip = iget(fd, inumber);

    mip->INODE.i_mode = S_IFLNK;		/* DIR and permissions */
    mip->INODE.i_uid  = running.uid;	/* Owner Uid */
    mip->INODE.i_gid =  running.gid;	/* Group Id */
    mip->INODE.i_size = strlen(link) ;		/* Size in bytes */

    mip->INODE.i_links_count = 1;	/* Links count */

    mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L);

    mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
    mip->dirty = 1;               /* mark dirty */

    for (i=0; i<15; i++)
    {
        mip->INODE.i_block[i] = 0;
	}

	strcpy(mip->INODE.i_block, link);
    iput(mip);

    ino = pip->ino;
    block_num = ino / super.s_inodes_per_group;
    offset = ino - (block_num * super.s_inodes_per_group) -1;
    lseek(fd, (1024 + (gd.bg_inode_table - 1) * 1024) + offset * sizeof(struct ext2_inode), 0);
    read(fd, &inode, sizeof( struct ext2_inode));

    get_block(fd,inode.i_block[0], buff);
    cp = buff;
    dp = (struct ext2_dir_entry_2* )buff;
    name_len = strlen(name);

    need_length = 4*((8 + name_len + 3)/4);

    while (size < inode.i_size)
    {
        cp += dp->rec_len;
        size += dp->rec_len;
        //dp_prev = dp;
        dp = (struct ext2_dir_entry_2*)cp;
        if (dp->rec_len + size == BLOCK_SIZE)
        {
            break;
		}
    }
    ideal_length = 4*((8+dp->name_len+3)/4);

    if(dp->rec_len - ideal_length >= need_length)
    {
        dp->rec_len = ideal_length;

        cp += dp->rec_len;
        size += dp->rec_len;
        dp = (struct ext2_dir_entry_2*)cp;

        dp->rec_len = BLOCK_SIZE - size;
        strcpy(dp->name, name);
        dp->name_len = name_len;
        dp->inode = inumber;
        dp->file_type = 2;
    }

    put_block(fd, inode.i_block[0], buff);
	pip->dirty = 1;
    
    iput(pip);
}
