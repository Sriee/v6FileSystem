/*
 * Cpin.h
 *
 *  Created on: 02-Dec-2015
 *      Author: sriee
 */

#ifndef CPIN_H_
#define CPIN_H_

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class Cpin {
	fstream fileSystem,eStream;
	string external, internal;
	int numberOfBlocksNeeded,fileSize;
public:
	void copyFile(string,string);
	bool checkParameters(string);
	void setExternalFile(string);
	string getExternalFile(void);
	void setInternalFile(string);
	string getInternalFile(void);
	bool isSmallFile(void);
	void copySmallFile(void);
	bool copyLargeFile(void);
	int getNextFreeBlock(void);
	int getLastBlockUsed(void);
};

#endif /* CPIN_H_ */
