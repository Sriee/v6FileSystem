/*
 * Cpout.h
 *
 *  Created on: 03-Dec-2015
 *      Author: sriee
 */

#ifndef CPOUT_H_
#define CPOUT_H_
#include<string>
#include <fstream>

using namespace std;
class Cpout {
	fstream fileSystem,eStream;
	string external, internal;
	int blocks;
	unsigned long long int fileSize;
public:
	void copyOutFile(string,string);
	bool checkParameters(string);
	void setExternalFile(string);
	string getExternalFile(void);
	void setInternalFile(string);
	string getInternalFile(void);
	void setBlocksToRead(int n);
	void calBlocks(int);
	int getBlocksToRead(void);
	bool searchFile(void);
};

#endif /* CPOUT_H_ */
