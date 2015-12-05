/*
 * Cpout.cpp
 *
 *  Created on: 03-Dec-2015
 *      Author: sriee
 */

#include "Cpout.h"
#include "MetaDirectory.h"
#include "Constants.h"
#include <fstream>
#include <iostream>

using namespace std;

/**
 *Used to check whether the parameters entered for 'cpout' command are valid
 *
 *@param inp String input given in the terminal
 *@return true - If parameters are valid for 'cpout' command
 *@return false - If parameters are invalid
 */
bool Cpout :: checkParameters(string inp){
	bool validCP = FALSE;
	string eFile,iFile;
	int space[2];
	space[0] = inp.find(" ");
	space[1] = inp.find(" ",space[0]+1);
	space[2] = inp.find(" ",space[1]+1);
	iFile = inp.substr(space[0]+1,space[1]-space[0]-1);
	eFile = inp.substr(space[1]+1,space[2]-space[1]-1);

	if ((eFile.length() <=0) | (iFile.length()<=0) | (space[0]<=0) | (space[1]<=0) ){
		cout <<"!!Entered Invalid number of arguments!!!" <<endl;
		validCP = FALSE;
	}else{
		setInternalFile(iFile);
		setExternalFile(eFile);
		validCP = TRUE;
	}
	return validCP;
}

/**
 * Entry point to this class. Checks if the parameters entered are valid and
 * does the basic setup required for copying file
 *
 * @param inp String input given in the terminal
 * @param path File System path that is set using 'initfs' command
 */
void Cpout :: copyOutFile(string inp,string path){
	string src,des;
	try{
		if(checkParameters(inp)){
			src = getInternalFile();
			des = getExternalFile();
			fileSystem.open(path.c_str(),ios::binary | ios::in );
			if(fileSystem.is_open()){
				if(searchFile()){
					cout <<"!!Copied File contents from " <<src <<" to " <<des <<" !!" <<endl;
				}else
					cout <<"!!Could not find " <<src <<" file!!!"  <<endl;
			} else cout<<"!!Could not find File System file!!" <<endl;
			fileSystem.clear();
			fileSystem.close();
		}
	 }catch(exception& e){
		 cout <<"Exception occured at Cpin :: Copy File" <<endl;
	 }

}

/**
 * Searches MetaDirectory class to check whether the file exits in the mv6 file system
 * iNode of the file is accessed, traverses to the datablock and starts copying from
 * internal file system to the output file.
 *
 * @return true If copying was successfull
 * @return false If copying was unsuccessfull
 */
bool Cpout :: searchFile(){
	bool found = FALSE;
	string ifile = getInternalFile();
	string efile = getExternalFile();

	int i=0,startAddr;

		for(;i<=MetaDirectory::Instance()->inodeCounter;i++){
			if(MetaDirectory::Instance()->entryName[i].compare(ifile)==0) break;
		}

		if(i>=MetaDirectory::Instance()->inodeCounter){
			found = FALSE;
			return found;
		}

		//get inode for the v6 file
		int v6node = MetaDirectory::Instance()->inodeList[i];

		iNode vInode;
		fileSystem.seekg(BLOCK_SIZE + (v6node*(int)sizeof(iNode)),ios::beg);
		fileSystem.read((char *)&vInode,sizeof(iNode));

		//Get file size and starting address
		if((vInode.flags & 0x9200) == 0x9200){
		 fileSize =((vInode.flags & 0x0040)| vInode.size0 <<15 | vInode.size1);
		}else if((vInode.flags & 0x8000) == 0x8000){
		 fileSize =(vInode.size0 <<16 | vInode.size1);
		}else{
			found = FALSE;
			return found;
		}
		calBlocks(fileSize);
		if(fileSize <= 4096) startAddr = vInode.addr[0];
		else{
			unsigned short *one,*two;
			one = new unsigned short[256];
			two = new unsigned short[256];
			fileSystem.seekg(vInode.addr[0]*BLOCK_SIZE,ios::beg);
			fileSystem.read((char *)&one,256  * sizeof(unsigned short));
			fileSystem.seekg(one[0]*BLOCK_SIZE,ios::beg);
			fileSystem.read((char *)&two,256  * sizeof(unsigned short));
			startAddr = two[0];
			found = TRUE;
		}

		eStream.open(efile.c_str(),ios::out | ios::app);
		int blk = getBlocksToRead();
		if(eStream.is_open()){
			for(int m=0;m<blk;m++){
				fileSystem.seekg(BLOCK_SIZE*startAddr,ios::beg);
				char *buffer = new char[BLOCK_SIZE];
				fileSystem.read(buffer,BLOCK_SIZE);
				eStream.seekg(m * BLOCK_SIZE,ios::beg);
				cout <<eStream.tellg() <<endl;
				eStream.write(buffer,BLOCK_SIZE);
				startAddr--;
			}
			found = TRUE;
			eStream.close();
		}
return found;
}

/**
 * Setter method to set the blocks required to be the read from the mv6 file system to the external
 * file
 *
 * @param n Number of blocks to read
 */
void Cpout :: setBlocksToRead(int n){
	this->blocks = n;
}

/**
 * Calculates the blocks that are required to read
 *
 * @param fsize Size of the file in mv6 file system
 */
void Cpout :: calBlocks(int fsize){
	int size;
	size = fsize/BLOCK_SIZE;
	if(fsize % BLOCK_SIZE !=0) size++;
	this->setBlocksToRead(size);
}

/**
 * Getter method to get the blocks required to be the read from the mv6 file system
 *
 * @return Number of blocks to read
 */
int Cpout :: getBlocksToRead(){
	return this->blocks;
}

/**
 * Setter method to get external file name
 *
 * @param name Name of the external file
 */
void Cpout :: setExternalFile(string name){
	this->external = name;
}

/**
 * Getter method to get external file name
 *
 * @return Name of the external file
 */
string Cpout :: getExternalFile(void){
	return this->external;
}

/**
 * Setter method to get internal file name
 *
 * @param name Name of the internal file
 */
void Cpout :: setInternalFile(string name){
	this->internal = name;
}


/**
 * Getter method to get internal file name
 *
 * @return Name of the internal file
 */
string Cpout :: getInternalFile(void){
	return this->internal;
}
