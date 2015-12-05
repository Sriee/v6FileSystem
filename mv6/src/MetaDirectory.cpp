/*
 * MetaDirectory.cpp
 *
 *  Created on: 03-Dec-2015
 *      Author: sriee
 */

#include <iostream>
#include <string>
#include <fstream>
#include "MetaDirectory.h"
#include "Constants.h"

using namespace std;

MetaDirectory* MetaDirectory :: m_Instance = NULL;

/**
 * Method to include the file entry to the directory
 *
 * @param entry Name of the entry. Entry could be a file or a directory
 * @param node iNode for the file or directory
 */
void MetaDirectory :: file_entry(string entry,int node){
		entryName[inodeCounter]= entry;
		inodeList[inodeCounter] = node;
		inodeCounter++;
}

/**
 * Helper method to return the instance of this singleton class
 */
MetaDirectory* MetaDirectory :: Instance(){
   if (!m_Instance)   // Only allow one instance of class to be generated.
	      m_Instance = new MetaDirectory;
   return m_Instance;
}

/**
 * Initializes the state of the directory. Called only once by initfs method
 */
void MetaDirectory :: dirState(){
	entryName = new string[numInodes+1];
	inodeList = new int[numInodes];
	entryName[inodeCounter] = path; // file system name for every directory, including root
	inodeList[inodeCounter]=1;// inode of root is 1
	inodeCounter++;
	entryName[inodeCounter] = path;
	inodeList[inodeCounter]=1;
	inodeCounter++;
}

/**
 * Gets the next free node available from the total number of inodes
 *
 * @return freeInode
 */
int MetaDirectory :: getNextFreeInode(void){
	int freeInode=0;
	if(inodeCounter == getInodeNum()) freeInode = -1;
	else freeInode = inodeCounter;
	return freeInode;
}

/**
 * When 'q' command is entered by the user this method is called
 * Based on the iNode count root node is written
 */
void  MetaDirectory :: quit(void){
	fileSystem.open(getPath().c_str(),ios::binary | ios::in | ios::out);
	if(fileSystem.is_open()){
		if(inodeCounter > 256) bigRoot();
		else smallRoot();
	} else cout<<"!!Could not find File System file!!" <<endl;
	fileSystem.clear();
	fileSystem.close();
}

/**
 * Writing root node with its iNode count is within 8*32
 */
void MetaDirectory :: smallRoot(){
	iNode rootNode;
	Directory writeDir;
	fileSystem.seekg(BLOCK_SIZE,ios::beg);
	fileSystem.read((char *)&rootNode,sizeof(iNode));
	rootNode.flags = (rootNode.flags | 0x9000);
	int nodeAllocated = 0,blocksNeeded;

	blocksNeeded = inodeCounter / numDirectoryEntry;
	if(inodeCounter % numDirectoryEntry !=0) blocksNeeded++;

	for(int i=0;i<8;i++){
		for(int j=0;(j<numDirectoryEntry) && (nodeAllocated<=inodeCounter);j++){
			rootNode.addr[j] =getNextFreeBlock();
			nodeAllocated+=32;
		}
	}

	nodeAllocated =0;
	for(int i=0;i<8;i++){
		if(rootNode.addr[i] !=0){
			fileSystem.seekg(rootNode.addr[i] * BLOCK_SIZE,ios::beg);
			for(int k; (k<=numDirectoryEntry) && (nodeAllocated++<=inodeCounter); k++){
				writeDir.inodeNumber=inodeList[inodeCounter];
				strcpy(writeDir.fileName,entryName[inodeCounter].c_str());
				fileSystem.write((char *)&writeDir,sizeof(Directory));
			}
		}else break;
	}
}

/**
 *Writes root when the inode count exceeds 8*32
 */
void MetaDirectory :: bigRoot(){
	iNode rootNode;
	Directory writeDir;
	fileSystem.seekg(BLOCK_SIZE,ios::beg);
	fileSystem.read((char *)&rootNode,sizeof(iNode));
	rootNode.flags = (rootNode.flags | 0xD000);
	int nodeAllocated = 0,blocksNeeded,blockAllocated=0;

	blocksNeeded = inodeCounter / numDirectoryEntry;
	if(inodeCounter % numDirectoryEntry !=0) blocksNeeded++;

	int addrRequired = blocksNeeded/131072;
	if(blocksNeeded % 131072!=0) addrRequired++;

	//Writing iNodes
	for(int i=0;i<8;i++) rootNode.addr[i] =getNextFreeBlock();
	fileSystem.seekg(BLOCK_SIZE,ios::beg);
	fileSystem.write((char *)&rootNode,sizeof(iNode));

	unsigned short *assignBlocks;
	assignBlocks= new unsigned short[256];

	for(int i=0;i<addrRequired;i++){
		int j=0;
		for(;(j<256) && (blockAllocated<=blocksNeeded);j++){
			assignBlocks[j] =getNextFreeBlock();
			blockAllocated+=32;
		}

		//add zeros if less than 256
		for(;j<256;j++)	assignBlocks[j] = 0;

		fileSystem.seekg(rootNode.addr[i] * BLOCK_SIZE,ios::beg);
		fileSystem.write((char *)&assignBlocks,256 * sizeof(unsigned short));
	}

	//Writing Double indirect blocks
	int start = ((rootNode.addr[addrRequired-1])-1);
	int stop = getLastBlockUsed();
	assignBlocks= new unsigned short[256];
	blockAllocated = 0;
	int j=0;
	for(int i=start; i>=stop; i--){
		for(;(j<256) && (blockAllocated<=blocksNeeded);j++){
			assignBlocks[j]=getNextFreeBlock();
			//Writing data
			fileSystem.seekg(BLOCK_SIZE * assignBlocks[j],ios::beg);
			for(int k; (k<=numDirectoryEntry) && (nodeAllocated++<=inodeCounter); k++){
				writeDir.inodeNumber=inodeList[inodeCounter];
				strcpy(writeDir.fileName,entryName[inodeCounter].c_str());
				fileSystem.write((char *)&writeDir,sizeof(Directory));
			}
			nodeAllocated++;
		}
		for(;j<256;j++) assignBlocks[j] = 0;
		fileSystem.seekg(i * BLOCK_SIZE,ios::beg);
		fileSystem.write((char *)&assignBlocks,256 * sizeof(unsigned short));
	}
}

/**
 * Setter method to set the path of file system
 *
 * @param loc path of file system
 */
void MetaDirectory :: setPath(string loc){
	path = loc;
}

/**
 * Getter method to get the path of file system
 *
 * @returns path of file system
 */
string MetaDirectory :: getPath(void){
	return path;
}

/**
 * Setter method to set the number of inodes
 *
 * @param num number of inodes
 */
void MetaDirectory :: setNumInodes(int num){
	numInodes = num;
}


/**
 * Getter method to get the number of inodes
 *
 * @return number of inodes
 */
int MetaDirectory :: getInodeNum(void){
	return numInodes;
}


/**
 * Finds a free block to be allocated for the new directory entry
 *
 * @throws Exception when unkown block is reached, invalid block is traveresed
 * @return free block
 */
int MetaDirectory :: getNextFreeBlock(void){
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
int MetaDirectory :: getLastBlockUsed(void){
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
