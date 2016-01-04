# v6FileSystem

<b>Objective </b><br>
To design and develop a program called <b>fsaccess</b> which will allow a Unix user access to the file system of a foreign operating system, the modified Unix Version 6 or mV6. 

This modified file system will support the following commands <br>

1. Initfs - For initializing the file system. <br>
2. cpin   - Creating a file using modified v6 file system. <br>
3. cpout  - For copying the contents of a file in the file system to an external file <br>
4. mkdir  - Creating directory using this file system. <br>
5. q      - Save and quit the file system. <br>

<b>Format of Commands and their functionalities </b><br>

1. <b>initfs /path/in/disk n1 n2. </b><br>
	n1 - represents the total number of blocks in the disk. <br>
	n2 - represents the total number of i-nodes in the disk.<br>

2. <b>cpin externalfile mv6-file </b><br>
	This command should create a new file called v6-file in the current directory of the v6 file system and fill the contents of the      newly created file with the contents of the external file. <br>
 
3. <b>cpout mv6-file externalfile </b><br> 
	If v6-file exists, the command will create external file and copies the contents from v6-file to the external file. If v6-file is     not present in the file system it will display an error message in the console. <br>

4. <b>mkdir mv6-dir </b> <br>
	If v6-dir is not already present in the v6 file system, a new directory is created. If the directory is already present in the file   system, an error message will be displayed in the console. <br>

5. <b>q</b><br>
	Save all changes and quit.

