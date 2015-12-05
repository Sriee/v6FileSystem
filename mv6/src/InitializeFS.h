/*
 * InitializeFS.h
 *
 *  Created on: 24-Nov-2015
 *      Author: sriee
 */
#ifndef INITIALIZEFS_H_
#define INITIALIZEFS_H_

#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "Constants.h"
#include "MetaDirectory.h"

using namespace std;


class InitializeFS {
	fstream file;
	string fsPath;
	int numOfBlocks,numOfinodes;
public:
	bool checkParameters(string);
	void setFileSystemPath(string);
	string getFileSystemPath(void);
	void setNumOfBlocks(int);
	int getNumOfBlocks(void);
	void setNumOfInodes(int);
	int getNumOfInodes(void);
	int getSizeOfInode(void);
	void createFileSystem(string);
	int getInodesBlock(void);
	int getFreeBlocks(void);
	int getFreeBlocksIndex(void);
	int calculateInodePadding(void);
	void readBlocks(void);	//Used to test the written data
};

#endif /* INITIALIZEFS_H_ */
