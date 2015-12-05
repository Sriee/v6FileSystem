/*
 * MakeDir.h
 *
 *  Created on: 26-Nov-2015
 *      Author: sriee
 */

#ifndef MAKEDIR_H_
#define MAKEDIR_H_
#include "InitializeFS.h"

class MakeDir {
	string dirname;
	fstream fileSystem;
public:
	void setDirectoryName(string);
	string getDirectoryName(void);
	bool searchDirectoryEntries(void);
	void allocateFreeDirectoryEntry(void);
	void createDirectory(string,string);
	bool checkParameters(string);
};

#endif /* MAKEDIR_H_ */
