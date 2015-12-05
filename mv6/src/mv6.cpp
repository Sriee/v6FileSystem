//============================================================================
// Name        : mv6.cpp
// Author      : Sriee
// Description : Entry function of the project. Based on user commands each
//				 commands operation is executed in the separate class
//============================================================================

#include <fstream>
#include <string>
#include "MakeDir.h"
#include "Cpin.h"
#include "Cpout.h"
#include "MetaDirectory.h"

using namespace std;

/**
 *Checks and validates the commands - initfs, cpin, cpout, mkdir and q
 */
class Entry{
	string command,path;
	int commandType,inodes,blocks;
	fstream file;
public:
	bool isValidCommand(string);
	void setCommandType(int value);
	int getCommandType();
};

/**
 * Setter method to set the command type
 *
 * @param value: integer value corresponding to the command
 */
void Entry::setCommandType(int value){
	this->commandType = value;
}

/*
 * Getter method to get the integer value for the command
 *
 * @return CommandType value
 */
int Entry::getCommandType(){
	return this->commandType;
}

/**
 * Checks whether the command is a valid entry
 *
 * @param inp: string input from the terminal
 */
bool Entry::isValidCommand(string inp){
	bool valid = false;
	this->command = inp.substr(0,inp.find(" "));

	if (INITFS.compare(this->command)==0){
		valid = true;
		this->setCommandType(static_cast<int>(initfs));
	}else if (CP_IN.compare(this->command)==0){
		valid = true;
		this->setCommandType(static_cast<int>(cpin));
	}else if (CP_OUT.compare(this->command)==0){
		valid = true;
		this->setCommandType(static_cast<int>(cpout));
	}else if (MK_DIR.compare(this->command)==0){
		valid = true;
		this->setCommandType(static_cast<int>(mkdir));
	}else if (Q.compare(this->command)==0){
		valid = true;
		this->setCommandType(static_cast<int>(q));
	}else{
		valid = false;
	}
	return valid;
}

/**
 * Gets inputs from the user via the terminal. Validates the command
 * Each command functionality is handled in separate files. Command
 * keeps executing until 'q' character is pressed.
 *
 */
int main(int argc,char *args[]) {
	string input,cmd,path;
	bool quit = false;

	Entry entry;		//Creating class instances
	InitializeFS fs;
	Cpin in;
	MakeDir mkd;
	Cpout out;

	cout <<"***V6 File System***" <<endl;
	cout <<"Press 'q' to quit" <<endl;

	while(!quit){
		getline(cin,input);

		if(entry.isValidCommand(input)) {
			switch(entry.getCommandType()){

				case initfs: fs.createFileSystem(input);
							 path = "";
							 path = (fs.getFileSystemPath()).append("fsaccess");
							 quit=false;
							 break;
				case cpin:   in.copyFile(input,path);
							 quit = false;
							 break;
				case cpout:  out.copyOutFile(input,path);
							 quit = false;
							 break;
				case mkdir:  mkd.createDirectory(input,path);
							 quit = false;
							 break;
				case q:
							 cout <<"!!!Quitting File system!!!" <<endl;
							 quit = true;
							 break;
				default: break;
			}
		}
		else cout<<"!!Entered Invalid Command!!\tTry Again...." <<endl;
	}
	return 0;
}
