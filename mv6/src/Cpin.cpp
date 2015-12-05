/*
 * Cpin.cpp
 *
 *  Created on: 02-Dec-2015
 *      Author: sriee
 */

#include "Cpin.h"
#include "Constants.h"
#include "MetaDirectory.h"

/**
 *Used to check whether the parameters entered for 'cpin' command are valid
 *
 *@param inp String input given in the terminal
 *@return true - If parameters are valid for 'cpin' command
 *@return false - If parameters are invalid
 */
bool Cpin :: checkParameters(string inp){
	bool validCP = FALSE;
	string eFile,iFile;
	int space[2];
	space[0] = inp.find(" ");
	space[1] = inp.find(" ",space[0]+1);
	space[2] = inp.find(" ",space[1]+1);
	eFile = inp.substr(space[0]+1,space[1]-space[0]-1);
	iFile = inp.substr(space[1]+1,space[2]-space[1]-1);

	if ((eFile.length() <=0) | (iFile.length()<=0) | (space[0]<=0) | (space[1]<=0)){
		cout <<"!!Entered Invalid number of arguments!!!" <<endl;
		validCP = FALSE;
	}else{
		setExternalFile(eFile);
		setInternalFile(iFile);
		validCP = TRUE;
	}
	return validCP;
}

/**
 * Checks whether the file referrred is small file or not. It then calls
 * approriate helper methods
 *
 * @param inp String input given in the terminal
 * @param path File System path that is set using 'initfs' command
 * @throws Exception when unkown block is reached, invalid block is traveresed
 */
void Cpin :: copyFile(string inp,string path){
string src,des;
try{
	if(checkParameters(inp)){
		src = getExternalFile();
		des = getInternalFile();
		fileSystem.open(path.c_str(),ios::binary | ios::in | ios::out);
		if(fileSystem.is_open()){
			eStream.open(getExternalFile().c_str(),ios::in | ios::ate);
			if(eStream.is_open()){
				if(isSmallFile()){ //Small File
					copySmallFile();
					cout <<"!!Copied File contents from " <<src <<" to " <<des <<"!!" <<endl;
				}else{ //Large File
					if(copyLargeFile())	cout <<"!!Copied File contents from " <<src <<" to " <<des <<"!!" <<endl;
					else cout <<"!!Error with copying files!!" <<endl;
				}
			eStream.clear();
			eStream.close();
			}else cout<<"!!Could not find " <<src <<" file!!" <<endl;
		} else cout<<"!!Could not find File System file!!" <<endl;
		fileSystem.clear();
		fileSystem.close();
	}
 }catch(exception& e){
	 cout <<"Exception occured at Cpin :: Copy File" <<endl;
 }
}

/**
 * Copies the content of the small file to mV6 file system
 *
 */
void Cpin :: copySmallFile(void){
	iNode newInodeToAllocate;
	int sInode;
	unsigned short lsb;
	char msb,dummy;

	char *fileContents = new char[fileSize];
	eStream.read(fileContents,fileSize);
	newInodeToAllocate.flags = (newInodeToAllocate.flags | 0x8000);
	//Storing size
	lsb = (fileSize & 0xffff);
	dummy= (int)(fileSize >> 16);
	msb = static_cast<unsigned int>(dummy);
	newInodeToAllocate.size1 = lsb;
	newInodeToAllocate.size0 = msb;

	for(int i=0;i<numberOfBlocksNeeded;i++) newInodeToAllocate.addr[i] = getNextFreeBlock();
	//Filling 0's for remaining addr array
	for(int i=numberOfBlocksNeeded;i<8;i++) newInodeToAllocate.addr[i] = 0;
	//Writing iNodes
	sInode = MetaDirectory::Instance()->getNextFreeInode();
	fileSystem.seekg(0,ios::beg);
	fileSystem.seekg(BLOCK_SIZE + (sInode * (int)sizeof(iNode)));
	fileSystem.write((char *)&newInodeToAllocate,sizeof(iNode));


	//Writing data
	fileSystem.seekg(0,ios::beg);
	fileSystem.seekg(BLOCK_SIZE * newInodeToAllocate.addr[0]);
	fileSystem.write(fileContents,fileSize);

	MetaDirectory::Instance()->file_entry(getInternalFile(),sInode);
}

/**
 * Copies the content of the small file to mV6 file system
 *
 * @return true - If file copy was successfull
 * @return false - If file copy was unsuccessfull
 */
bool Cpin :: copyLargeFile(void){
	bool large = FALSE;
	iNode newInodeToAllocate;
	int thisInode;
	unsigned short lsb;
	char msb,dummy;
	int addrRequired = numberOfBlocksNeeded/131072;

	if(numberOfBlocksNeeded % 131072!=0) addrRequired++;

	if(addrRequired > 8){	//If file size exceeds maximum
		cout<<"!!File size exceeds maximum size limit!!" <<endl;
		large = FALSE;
	}else{
		newInodeToAllocate.flags = (newInodeToAllocate.flags | 0x9000);
		for(int i=0;i<addrRequired;i++) newInodeToAllocate.addr[i] = getNextFreeBlock();
		//Filling 0's for remaining addr array
		for(int i=addrRequired;i<8;i++) newInodeToAllocate.addr[i] = 0;

		lsb = (fileSize & 0xffff);
		dummy= (int)(fileSize >> 16);
		if(dummy < 256)	msb = static_cast<unsigned int>(dummy);
		else{
			dummy= (int)(fileSize >> 15);
			msb = static_cast<unsigned int>(dummy);
			//Setting the 25th bit in flags field
			newInodeToAllocate.flags |= 0x9200;
		}
		newInodeToAllocate.size1 = lsb;
		newInodeToAllocate.size0 = msb;

		//Writing iNodes
		thisInode = MetaDirectory::Instance()->getNextFreeInode();
		fileSystem.seekg(0,ios::beg);
		fileSystem.seekg(BLOCK_SIZE + (thisInode * (int)sizeof(iNode)));
		fileSystem.write((char *)&newInodeToAllocate,sizeof(iNode));

		//Writing single indirect blocks
		int blocksAllocated = 0; 				// Keep track of num of addr array allocated
		unsigned short *assignBlocks;
		assignBlocks= new unsigned short[256];

		//write 256->512 addresses into the first indirect block
		for(int i=0;i<addrRequired;i++){
			int j=0;
			for(;(j<256) && (blocksAllocated<=numberOfBlocksNeeded);j++){
				assignBlocks[j] =getNextFreeBlock();
				blocksAllocated+=512;
			}

			//add zeros if less than 256
			for(;j<256;j++)	assignBlocks[j] = 0;

			fileSystem.seekg(newInodeToAllocate.addr[i] * BLOCK_SIZE,ios::beg);
			fileSystem.write((char *)&assignBlocks,256 * sizeof(unsigned short));
		}

		char *fileContents = new char[fileSize]; //reading input data


		//Writing Double indirect blocks
		int start = ((newInodeToAllocate.addr[addrRequired-1])-1);
		int stop = getLastBlockUsed();
		assignBlocks= new unsigned short[256];
		blocksAllocated = 0;
		int j=0;
		for(int i=start; i>=stop; i--){
			for(;(j<256) && (blocksAllocated<=numberOfBlocksNeeded);j++){
				assignBlocks[j]=getNextFreeBlock();

				//Writing data
				fileSystem.seekg(BLOCK_SIZE * assignBlocks[j],ios::beg);
				eStream.seekg(BLOCK_SIZE * j);
				eStream.read(fileContents,BLOCK_SIZE);
				fileSystem.write(fileContents,BLOCK_SIZE);
				blocksAllocated++;
			}
			for(;j<256;j++) assignBlocks[j] = 0;
			fileSystem.seekg(i * BLOCK_SIZE,ios::beg);
			fileSystem.write((char *)&assignBlocks,256 * sizeof(unsigned short));
		}

	large = TRUE;
	}

	MetaDirectory::Instance()->file_entry(getInternalFile(),thisInode);
	return large;
}

/**
 * Setter method to set the name of external file
 *
 * @param name Name of the external file
 */
void Cpin :: setExternalFile(string name){
	this->external = name;
}

/**
 * Getter method to get the name of external file
 *
 * @return Name of the external file
 */
string Cpin :: getExternalFile(void){
	return this->external;
}

/**
 * Setter method to set the name of internal file
 *
 * @param name Name of the internal file
 */
void Cpin :: setInternalFile(string name){
	this->internal = name;
}

/**
 * Getter method to get the name of internal file
 *
 * @return Name of the internal file
 */
string Cpin :: getInternalFile(void){
	return this->internal;
}

/**
 * Analysis the size of the file. Based on this calculation the control to routed to the helper
 * methods
 *
 * @return true If external file is a small file
 * @return false If external file is a large file
 */
bool Cpin :: isSmallFile(void){
	bool res = FALSE;
	fileSize = eStream.tellg();
	eStream.seekg(0,ios::beg);
	if(fileSize == 0) cout <<"!!Empty File!!" <<endl;
	else{
		numberOfBlocksNeeded = fileSize / BLOCK_SIZE;			 //calculate the the data blocks required to store the file
		if((fileSize % BLOCK_SIZE) !=0) numberOfBlocksNeeded++ ; //extra data lesser than a block size
		if(fileSize <= 4096) res = TRUE;
		else res = FALSE;
	}
	return res;
}

/**
 * Finds a free block to be allocated for the new directory entry
 *
 * @throws Exception when unkown block is reached, invalid block is traveresed
 * @return free block
 */
int Cpin :: getNextFreeBlock(void){
	int freeBlk,freeChainBlock;
	unsigned short freeHeadChain;
	superBlock freeBlockSB;
	try{
		//Moving the cursor to the start of the file
		fileSystem.seekg(0);

		//Reading the contents of super block
		fileSystem.read((char *)&freeBlockSB,sizeof(superBlock));

		if(freeBlockSB.nfree == 1){
			freeChainBlock = freeBlockSB.free[--freeBlockSB.nfree];

			//Head of the chain points to next head of chain free list
			fileSystem.seekg(freeChainBlock);
			fileSystem.read((char *)&freeHeadChain,2);

			//reset nfree to 1
			freeBlockSB.nfree = 100;

			//reset the free array to new free list
			for(int k=0;k<100;k++){
				freeBlockSB.free[k] = freeHeadChain + k;
			}

			freeBlk = (int)freeBlockSB.free[--freeBlockSB.nfree];

			//Writing after the free head chain is changed
			fileSystem.write((char *)&freeBlockSB,sizeof(superBlock));
		}else{
			freeBlk = (int)freeBlockSB.free[--freeBlockSB.nfree];
			//Writing after the free block has been allocated
			fileSystem.seekg(0,ios::beg);
			fileSystem.write((char *)&freeBlockSB,sizeof(superBlock));
		}
	}catch(exception& e){
		cout<<"Exception at getNextFreeBlock " <<endl;
	}
	return freeBlk;
}

/**
 * Finds the last block which has been allocated to the file system
 *
 * @throws Exception when unkown block is reached, invalid block is traveresed
 * @return last used block
 */
int Cpin :: getLastBlockUsed(void){
	int lastBlk;
	superBlock freeBlockSB;
	try{
		//Moving the cursor to the start of the file
		fileSystem.seekg(0);

		//Reading the contents of super block
		fileSystem.read((char *)&freeBlockSB,sizeof(superBlock));

		lastBlk = (int)freeBlockSB.free[freeBlockSB.nfree];
	}catch(exception& e){
		cout<<"Exception at getNextFreeBlock " <<endl;
	}
	return lastBlk;
}
