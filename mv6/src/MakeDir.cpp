/*
 * MakeDir.cpp
 *
 *  Created on: 26-Nov-2015
 *      Author: sriee
 */

#include "MakeDir.h"

/**
 *Checks whether the directory is already present in the file system. If not creates a new directory
 *entry
 *
 *@param inp String input given in the terminal
 *@param path File System path that is set using 'initfs' command
 */

void MakeDir :: createDirectory(string inp,string path){
	if(checkParameters(inp)){
		fileSystem.open(path.c_str(),ios::binary | ios::in | ios::out);
		if(fileSystem.is_open()){
			if(searchDirectoryEntries()){ //Searched Directory has been found
				cout <<"!!Directory present in mv6 File System!!" <<endl;
				return;
			}else{
				allocateFreeDirectoryEntry();
				cout <<"!!New Directory Entry allocated!!" <<endl;
			}
		fileSystem.clear();
		fileSystem.close();
		} else cout<<"!!Could not find File System file!!" <<endl;
	}
}

/**
 * Used to check whether the parameter entered for mkdir command is valid
 *
 *@return true - If the parameter is valid for mkdir command
 *@return false - If the parameter is invalid
 */
bool MakeDir :: checkParameters(string inp){
	bool validCP = FALSE;
	string name;
	int space[1];
	space[0] = inp.find(" ");
	space[1] = inp.find(" ",space[0]+1);
	name = inp.substr(space[0]+1,space[1]-space[0]-1);
	if((name.length()<=0) | (space[0]<=0)){
		cout <<"!!Entered Invalid number of arguments!!!" <<endl;
		validCP = FALSE;
	}else if(name.size()>13){
		cout <<"!!Maximum number of character supported for Directory name is 14!!" <<endl;
		cout <<"!!Resizing " <<name.size() <<" characters ";
		name.resize(14);
		cout <<"to 14 characters:" <<name <<endl;
		setDirectoryName(name);
		validCP = TRUE;
	}else{
		setDirectoryName(name);
		validCP = TRUE;
	}
	return validCP;
}

/**
 * Searches whether the directory already exists in the file system
 *
 * @return true - If the directory entry is present in the file system
 * @return false - If the directory entry is not present
 */
bool MakeDir :: searchDirectoryEntries(){
bool found = FALSE;
int i=0;

	for(;i<=MetaDirectory::Instance()->inodeCounter;i++){
			if(MetaDirectory::Instance()->entryName[i].compare(getDirectoryName())==0) {
				found = TRUE;
				break;
			}
	}

	if(i>=MetaDirectory::Instance()->inodeCounter){
		found = FALSE;
		return found;
	}

return found;
}

/**
 * Allocates new directory entry, assigns inode & free data block for the new directory entry
 * Traverses through single indirect and double indirect blocks to allocate directory entry
 *
 */
void MakeDir :: allocateFreeDirectoryEntry(){
iNode newInodeToAllocate;
Directory rootDirectory,myDirectory,filler;
int dnode;
	newInodeToAllocate.flags = (newInodeToAllocate.flags | 0x9000);
	dnode =  MetaDirectory::Instance()->getNextFreeInode();
	MetaDirectory::Instance()->file_entry(getDirectoryName(),dnode);
	//Writing iNodes
	fileSystem.seekg(0,ios::beg);
	fileSystem.seekg(BLOCK_SIZE + (dnode * (int)sizeof(iNode)));
	fileSystem.write((char *)&newInodeToAllocate,sizeof(iNode));

	//Writing directory
	fileSystem.seekg(0,ios::beg);
	fileSystem.seekg(BLOCK_SIZE * newInodeToAllocate.addr[0]);

	//Setting '.' character
	rootDirectory.inodeNumber=1;
	strcpy(rootDirectory.fileName,"..");
	fileSystem.write((char *)&rootDirectory,sizeof(rootDirectory));

	//Setting directory name
	string name = getDirectoryName();
	myDirectory.inodeNumber=dnode;
	strcpy(myDirectory.fileName,name.c_str());
	fileSystem.write((char *)&rootDirectory,sizeof(rootDirectory));

	//Setting the remaining directory entries
	for(int j=3; j<=numDirectoryEntry; j++){
		fileSystem.write((char *)&filler,sizeof(filler));
	}
}

/**
 *Setter method to save the directory name input from the user
 *
 *@param name directory name
 */
void MakeDir :: setDirectoryName(string name){
	this->dirname = name;
}

/**
 * Getter method to get the directory name given by the user
 *
 * @return dirname Directory name
 */
string MakeDir :: getDirectoryName(void){
	return this->dirname;
}
