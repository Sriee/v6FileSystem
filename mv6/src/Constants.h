#include <string>

/*
 * Constant assigned to the command names supported by modified v6 file system
 */
const std::string INITFS = "initfs";
const std::string CP_IN = "cpin";
const std::string CP_OUT = "cpout";
const std::string MK_DIR = "mkdir";
const std::string Q = "q";

/*
 * Constant assigned for block size
 */
const int BLOCK_SIZE=512;

/**
 * Constant Boolean values
 */
const bool TRUE = true;
const bool FALSE = false;

/*
 * Commands that are supported in the modified v6 file system
 */
enum CommandTypes{
	initfs=1,
	cpin,
	cpout,
	mkdir,
	q
};

/*
 * Super Block structure
 */
struct superBlock{
	unsigned short isize;		//Number of blocks denoted to inodes
	unsigned short fsize;		//Total number of blocks
	unsigned short nfree;
	unsigned short free[100];
	unsigned short ninode;		//Total number of inodes
	unsigned short inode[100];	//Not used
	char flock;					//Not used
	char ilock;					//Not used
	char fmod;					//Not used
	unsigned short time[2];		//Not used
	superBlock(){	//For initilizing the struct
		isize = 0 ;
		fsize = 0;
		nfree = 0;
		for(int i=0;i<100;i++){
			free[i] = 0;
			inode[i] = 0;
		}
		ninode = 0;
		flock = '0';
		ilock = '0';
		fmod = '0';
		time[0] = time[1] = 0;
	}
};

/*
 * I-Nodes structure
 */
struct iNode{
	unsigned short flags;
	char nlinks;			 	//Not used
	char uid;					//Not used
	char gid;					//Not used
	char size0;
	unsigned short size1;
	unsigned short addr[8];
	unsigned short actime[2];	//Not used
	unsigned short modtime[2];	//Not used

	iNode(){
		flags = 0;
		nlinks = '0';
		uid = '0';
		gid ='0';
		size0 ='0';
		size1 =0;
		for(int j=0;j<8;j++) addr[j] =0;
		actime[0] = actime[1] =0;
		modtime[0] = modtime[1] =0;
	}
};

//Directory Structure
struct Directory{
	unsigned short inodeNumber;
	char fileName[14];

	Directory(){
		inodeNumber =0;
		for(int k=0;k<14;k++) fileName[k] =0;
	}
};

/**
 * Constant number of directory entries in a data block
 */
const int numDirectoryEntry = (BLOCK_SIZE/sizeof(Directory));

