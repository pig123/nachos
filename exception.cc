void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    if (which == SyscallException) {
        switch (type) {
            case SC_Halt:
                DEBUG('a',"Shutdown, initated by user program.\n");
                interrupt->Halt();
            break;
            //we define some file operation function
            case SC_Create:
                DEBUG('a',"Create File\n");
                //first we get the name
                //key thing is the canshuchuandi.
                break;
            case SC_Open:
                break;
            case SC_Read:
                break;
            case SC_Write:
                break;
            case SC_Close:
                break;
            //we define some thread operation function
            case SC_Exec:
                break;
            case SC_Fork:
                break;
            case SC_Yield:
                break;
            case SC_Join:
                break;
            case SC_Exit:
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
