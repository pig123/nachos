void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    if (which == SyscallException) {
        switch (type) {
            case SC_Halt:
                DEBUG('a',"Shutdown, initated by user program.\n");
                interrupt->Halt();
            break;
            #ifdef FILESYS
	    //we define some file operation function
            case SC_Create:
                DEBUG('a',"Create File\n");
                //first we get the name
                char *filename;
		filename = (char *)machine->ReadRegister(4);
		fileSystem->Create(filename,0);
                break;
            case SC_Open:
	      DEBUG('a',"Open File\n");
	      char *filename;
	      int rescode;
	      filename = (char *)machine->ReadReigster(4);
	      rescode =(int) fileSystem->Open(filename);
	      machine->WriteRegister(2,rescode);
                break;
            case SC_Read:
	      DEBUG('a',"Read File\n");
	      buffer = (char *)machine->ReadReigster(4);
	      size = machine->ReadReigster(5);
	      int number;
	      OpenFile *fid = (OpenFile *)machine->ReadRegister(6);
	      number = fid->Read(buffer,size);
	      machine->WriteRegister(2,number);
              break;
            case SC_Write:
	      DEBUG('a',"Write File\n");
	      buffer = (char *)machine->ReadReigster(4);
	      size = machine->ReadReigster(5);
	      OpenFile *fid =(OpenFile *) machine->ReadReigster(6);
	      fid->Write(&buffer, size);
              break;
            case SC_Close:
	      DEBUG('a',"Close File\n");
	      OpenFile *fid;
	      fid = (OpenFile *)machine->ReadReisgter(4);
	      delete fid;
              break;
            #endif
	    //we define some thread operation function
            case SC_Exec:
	      DEBUG('a',"Exec the program!!\n")
		char *filename;
	      filename = (char *)machine->ReadReigster(4);
	      OpenFile *executable = fileSystem->Open(filename);
	      if (executable == NULL) {
		printf("Unable to open file\n");
		return;
	      }
	      space = new AddrSpace(executable);
	      currentThread->space = space;
	      delete  executable;
	      space->InitRegisters();
	      space->RestoreState();
	      machine->Run();
	      machine->WriteRegister(2,(int)space);
                break;
            case SC_Fork:
	      DEBUG('a',"Fork a thread \n");
	      // first we get the func addr;
	      func = machine->ReadReigster(4);
	      
	      currentThread->Fork(func, 0);
              break;
            case SC_Yield:
	      DEBUG('a',"Yield a thread\n");
	      currentThread->Yield();
              break;
            case SC_Join:
	      DEBUG('a',"Join a thread\n");
	      //int Join(SpaceId id)
	      //only return once the user program id has finished.
	      //Return the exit status.
	      AddrSpace *s = (AddrSpace *)machine->ReadRegister(4);
	      int status = currentThread->Join(s);
	      machine->WriteRegister(2,status);
	      //we call the thread2, then make the currentThread sleep
	      //first question is how to find the thread with the space.
	      //we iternate the threads in the threads_list,
	      // when the thread2 finished, we wake up the currentThread.
	      
                break;
            case SC_Exit:
	      DEBUG('a',"Exit.");
	      int status = machine->ReadRegister(4);
	      // we delete the space currently....
	      Thread *t1  = scheduler->FindNextToRun();
	      Thread *t2 = currentThread;
	      t2->Finish();
	      scheduler->Run(t1);
              break;
            default:
                printf("Unexpected user mode exception %d %d\n");
                ASSERT(FALSE);
        }
    } else {
        // do the other exception handler
        // like page fault???
        printf("Unexpected user mode exception %d %d\n",which ,type);
        ASSERT(FALSE);
    }
}
