/*
 * MetaDirectory.h
 *
 *  Created on: 03-Dec-2015
 *      Author: sriee
 */

#ifndef METADIRECTORY_H_
#define METADIRECTORY_H_

#include <string>
#include <cstring>

using namespace std;

/**
 * This class is based on "Singleton Design Pattern". This Singleton class
 * maintains the meta directory for the project. All other classes can access only
 * a single instance of the class. Keeps track of the file and directories added
 * to the file system.
 *
 */
class MetaDirectory {
private:
	//Kept private so only this class can have access to its instance
	static MetaDirectory* m_Instance;

public:
	string *entryName;
	string path;
	int *inodeList;
	int inodeCounter,numInodes;
	fstream fileSystem;

	MetaDirectory()	{
		entryName = NULL;
		path = "";
		inodeCounter = numInodes =0;
		inodeList = new int[0];
	}

	static MetaDirectory* Instance(); //Public static access function
	void dirState(void);
	void file_entry(string,int);
	void setPath(string path);
	void setNumInodes(int num);
	string getPath(void);
	int getInodeNum(void);
	int getNextFreeInode(void);
	void quit(void);
	void smallRoot(void);
	void bigRoot(void);
	int getLastBlockUsed(void);
	int getNextFreeBlock(void);
};

#endif /* METADIRECTORY_H_ */
