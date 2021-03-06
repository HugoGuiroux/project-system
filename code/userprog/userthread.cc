// userthread.cc
//      Test routines for demonstrating that Nachos can load
//      a user program and execute it.
//
//      Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "synchconsole.h"
#include "addrspace.h"
#include "synch.h"
#include "userthread.h"
#include "thread.h"


//----------------------------------------------------------------------
// StartThread
//      Run a user program.  Open the executable, load it into
//      memory, and jump to it.
//----------------------------------------------------------------------

void StartUserThread(int f)
{
    userfunc *uf = (userfunc*)f;
    machine->WriteRegister(PCReg,uf->funcWrapper); //put the function to the PC register
    machine->WriteRegister(NextPCReg,uf->funcWrapper+sizeof(int));

    machine->WriteRegister(4, uf->funcUser);	//put the user function to register 4
    machine->WriteRegister(5, uf->arg);	        //put the arg to register 5
    DEBUG('a', "value of stack : %d\n",machine->ReadRegister(StackReg));

    machine->Run();
    return;
}

int do_UserThreadCreate(int fnWrapper, int fnUser, int arg)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);      //Block Interrupt to be atomic

    // Does not go above MAX_TOTAL_THREADS
    if (currentThread->space->CurrentThreadNumber() > MAX_TOTAL_THREADS)
    {
        (void) interrupt->SetLevel (oldLevel);
        return -2;
    }

    Thread* t = new  Thread("NewThread");
    t->uf = new userfunc;				//define the struct

    t->uf->funcWrapper = fnWrapper;                                 //pack the functionWrapper
    t->uf->funcUser = fnUser;
    t->uf->arg = arg;                                //pack the argument

    int stack = currentThread->space->GetNewUserStack();
    if(stack == 0)
    {
        (void) interrupt->SetLevel (oldLevel);
        return -1;
    }

    int i;

    for (i = 0; i < NumTotalRegs; i++)
        t->userRegisters[i] = 0;

    t->userRegisters[StackReg] = stack;
    // Also set frame pointer to point to stack
    t->userRegisters[FramePointer] = stack;
    t->userStack=stack;
    DEBUG('a', "value of stack : %d\n", stack);

    t->Fork(StartUserThread, (int)t->uf);

    (void) interrupt->SetLevel (oldLevel);
    return t->GetTid();
}

void do_UserThreadExit()
{
    currentThread->space->SetThreadReturn(currentThread->GetTid(), machine->ReadRegister(4));
    DEBUG('t', "Setting return val of thread %d with %d\n", currentThread->GetTid(), currentThread->space->GetThreadReturn(currentThread->GetTid()));

    // If last thread, call exit
    if (currentThread->space->CurrentThreadNumber() > 1)
    {
        currentThread->space->DecThreadNumber();
        currentThread->Finish();
    }
    else
    {
        AddrSpace::Exit();
    }
}

int do_UserThreadJoin()
{
    // Get thread id
    int tid = machine->ReadRegister(4);

    // Get return @ for exit code
    int retval = machine->ReadRegister(5);

    // Find thread with this id
    Thread* target = currentThread->space->GetThreadById(tid);
    if (target == NULL)
        return -1;

    // If thread not already ended, wait for it
    if (!currentThread->space->ThreadEnded(tid))
    {
        // Semaphore are interruptible, can join here
        // If already thread joining, can't join also
        if (!target->Join(currentThread))
            return -2;
    }

    // Fill user return
    int retThread = currentThread->space->GetThreadReturn(tid);

    // Fill retval with return value if not null
    DEBUG('t', "Thread exit with return %d for address %d\n", retThread, retval);
    if (retval != 0)
        machine->WriteMem(retval, 4, retThread);


    return 0;
}
//should be in the process we want to create now
void StartProc(int name)
{
    char *filename = (char *)name;

    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL)
    {
        printf ("Unable to open file %s\n", filename);
        return;
    }
    currentThread->space->LoadExecutable(executable);

    delete executable;

    currentThread->space->InitRegisters ();	// set the initial register values
    currentThread->space->RestoreState ();	// load page table register
    machine->Run ();		// jump to the user progam
    ASSERT (FALSE);		// machine->Run never returns;
}
