// directory.cc 
//	Routines to manage a directory of file names.
//
//	The directory is a table of fixed length entries; each
//	entry represents a single file, and contains the file name,
//	and the location of the file header on disk.  The fixed size
//	of each directory entry means that we have the restriction
//	of a fixed maximum size for file names.
//
//	The constructor initializes an empty directory of a certain size;
//	we use ReadFrom/WriteBack to fetch the contents of the directory
//	from disk, and to write back any modifications back to disk.
//
//	Also, this implementation has the restriction that the size
//	of the directory cannot expand.  In other words, once all the
//	entries in the directory are used, no more files can be created.
//	Fixing this is one of the parts to the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

//----------------------------------------------------------------------
// Directory::Directory
// 	Initialize a directory; initially, the directory is completely
//	empty.  If the disk is being formatted, an empty directory
//	is all we need, but otherwise, we need to call FetchFrom in order
//	to initialize it from disk.
//
//	"size" is the number of entries in the directory
//----------------------------------------------------------------------

Directory::Directory()
{
  /*
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++)
	table[i].inUse = FALSE;
  */
  //we init the Directory.
  root = new DirectoryEntry();
  //we add two file . .. in the root dir.
  root->inUse = False;
  root->name = "root";
  root->type = 1;
  root->leftchild = NULL;
  root->rightchild = NULL;
  root->parentdir = root;
  root->parententry = root;
  int type = 0;
  //if we init, we need this two file.
  Add("/.",type,3);
  Add("/..",type,3);
}

//----------------------------------------------------------------------
// Directory::~Directory
// 	De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{ 
    delete [] table;
} 

//----------------------------------------------------------------------
// Directory::FetchFrom
// 	Read the contents of the directory from disk.
//
//	"file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::WriteBack
// 	Write any modifications to the directory back to disk
//
//	"file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::FindIndex
// 	Look up file name in directory, and return its location in the table of
//	directory entries.  Return -1 if the name isn't in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
	    return i;
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
// 	Look up file name in directory, and return the disk sector number
//	where the file's header is stored. Return -1 if the name isn't 
//	in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::Find(char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].sector;
    return -1;
}

//----------------------------------------------------------------------
// Directory::Add
// 	Add a file into the directory.  Return TRUE if successful;
//	return FALSE if the file name is already in the directory, or if
//	the directory is completely full, and has no more space for
//	additional file names.
//
//	"name" -- the name of the file being added
//  "type" --  two point if add is a dir or a file.
//	"newSector" -- the disk sector containing the added file's header
//----------------------------------------------------------------------

bool
Directory::Add(char *name, int type ,int newSector)
{ 
  char *tmp_name = (char *)malloc(strlen(name));
  strcpy(tmp_name, name);
  DirectoryEntry *current_path = GetCurrentPath(name);
  DirectoryEntry *parent_path = GetParentPath(name);
  DirectoryEntry *current;
  char *filename = GetFileName(name);
  if (type == 1) {
	//add a dir
	DirectoryEntry *dir = new DirectoryEntry();//
	DirectoryEntry *tmp;
	tmp = current_path->leftchild;
	while(tmp->rightchild!=NULL)tmp = tmp->rightchild;
	
	dir->name = tmp_name;
	dir->leftchild = NULL;
	dir->rightchild = NULL;
	dir->parentdir = NULL;
	tmp->rightchild = dir;
	dir->parent = tmp;
	
	char tmp_path[1024];
	memset(tmp_path, 0,1024);
	sprintf(tmp_path,"%s%s",name,"/.");
	Add(tmp_path, 0, newSector);
	memset(tmp_path,0,1024);
	sprintf(tmp_path,"%s%s",name,"/..");
	Add(tmp_path,0,newSector);
  } else if (type == 0) {
    //add a file
	if (filename == '.') {
	  current = new DirectoryEntry();
	  current->type = 0;
	  current->name = tmp_name;
	  current->leftchild = NULL;
	  current->rightchild = NULL;
	  current->parentdir = current_path;
	  current_path->leftchild = current;
	  current->parent = current_path;
	  
	} else if (filename == "..") {
	  current = new DirectoryEntry();
	  current->type = 0;
	  current->name = tmp_name;
	  current->rightchild = NULL;
	  current->leftchild = NULL;
	  current->parentdir = parent_path;
	  current_path->leftchild->rightchild = current;
	  current->parent = current_path->leftchild;
	} else {
	  //other files.
	  if (FindIndex(name)!= -1) return FALSE;
	  current = new DirectoryEntry();
	  DirectoryEntry *tmp;
	  current->type = 0;
	  current->name = tmp_name;
	  current->rightchild = NULL;
	  current->leftchild = NULL;
	  current->parentdir = NULL;
	  while(tmp->rightchild != NULL) tmp = tmp->rightchild;
	  tmp->rightchild = current;
	  current->parent = tmp;
	}
	/*
	if (FindIndex(name) != -1)
	return FALSE;
    int filenamelen = strlen(name);
    
    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = TRUE;
            table[i].name = (char *)malloc(filenamelen+1);
	    strncpy(table[i].name,name, filenamelen);
	    //strncpy(table[i].name, name, FileNameMaxLen); 
            
	    table[i].sector = newSector;
        return TRUE;
	}
    return FALSE;	// no space.  Fix when we have extensible files.
	*/
   }
}

//----------------------------------------------------------------------
// Directory::Remove
// 	Remove a file name from the directory.  Return TRUE if successful;
//	return FALSE if the file isn't in the directory. 
//
//	"name" -- the file name to be removed
//  "type" -- the type, type==1 dir, type==0 file
//----------------------------------------------------------------------

bool
Directory::Remove(char *name, int type)
{ 
  /*
    int i = FindIndex(name);

    if (i == -1)
	return FALSE; 		// name not in directory
    table[i].inUse = FALSE;
    return TRUE;	
  */
  
}

//----------------------------------------------------------------------
// Directory::List
// 	List all the file names in the directory. 
//----------------------------------------------------------------------

void
Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	if (table[i].inUse)
	    printf("%s\n", table[i].name);
}

//----------------------------------------------------------------------
// Directory::Print
// 	List all the file names in the directory, their FileHeader locations,
//	and the contents of each file.  For debugging.
//----------------------------------------------------------------------

void
Directory::Print()
{ 
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
	if (table[i].inUse) {
	    printf("Name: %s, Sector: %d\n", table[i].name, table[i].sector);
	    hdr->FetchFrom(table[i].sector);
	    hdr->Print();
	}
    printf("\n");
    delete hdr;
}
