// progtest.cc
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

//----------------------------------------------------------------------
// StartProcess
//      Run a user program.  Open the executable, load it into
//      memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess (char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL)
    {
        printf ("Unable to open file %s\n", filename);
        return;
    }
    space = new AddrSpace();
    space->LoadExecutable(executable);
    space->AttachThread(currentThread);

    delete executable;		// close file

    space->InitRegisters ();	// set the initial register values
    space->RestoreState ();	// load page table register

    machine->Run ();		// jump to the user progam
    ASSERT (FALSE);		// machine->Run never returns;
    // the address space exits
    // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
//      Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void
ReadAvail (int arg)
{
    readAvail->V ();
}
static void
WriteDone (int arg)
{
    writeDone->V ();
}

//----------------------------------------------------------------------
// ConsoleTest
//      Test the console by echoing characters typed at the input onto
//      the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console (in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore ("read avail", 0);
    writeDone = new Semaphore ("write done", 0);

    for (;;)
    {
        readAvail->P();	// wait for character to arrive
        ch = console->GetChar();

        if (ch != '\n')
        {
            console->PutChar('<');
            writeDone->P();
        }

        console->PutChar(ch);	// echo it!
        writeDone->P();	// wait for write to finish

        if (ch != '\n')
        {
            console->PutChar('>');
            writeDone->P();
        }

        if (ch == 'q' || ch == EOF)
            return;		// if q or EOF, quit
    }
}

//----------------------------------------------------------------------
// SynchConsoleTest
//      Test the synchronize console by echoing characters typed at the input
// onto the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void SynchConsoleTest(char *in, char *out)
{
    char ch;

    // Init console
    SynchConsole *sc = new SynchConsole(in, out);

    // Get & print char by char
    while ((ch = sc->SynchGetChar()) != EOF)
        sc->SynchPutChar(ch);

    // Should only go out loop when EOF
    fprintf(stderr, "Nachos: EOF detected in SynchConsole!\n");

    // Delete to test destructor
    delete sc;
}

void SynchConsoleTestChev(char *in, char *out)
{
    char ch;
    char string[4];

    string[0] = '<';
    string[2] = '>';
    string[3] = '\0';

    // Init Console
    SynchConsole *sc = new SynchConsole(in, out);

    // Get & print char by char
    while ((ch = sc->SynchGetChar()) != EOF)
    {
        string[1] = ch;
        sc->SynchPutString(const_cast<char*>(string));
    }

    // Should only go out loop when EOF
    fprintf(stderr, "Nachos: EOF detected in SynchConsole!\n");

    // Delete to test destructor
    delete sc;
}

void SynchConsoleTestString(char *in, char *out)
{
    char buffer[12];
    char ended = 0;

    buffer[10] = '\n';
    buffer[11] = '\0';

    // Init Console
    SynchConsole *sc = new SynchConsole(in, out);

    // Try to read at most 10 char & stop if
    do
    {
        sc->SynchGetString(buffer, 11);
        ended = buffer[10] != '\0';
        buffer[10] = '\n';
        sc->SynchPutString(const_cast<char*>(buffer));
    } while (!ended);

    // Should only go out loop when EOF
    fprintf(stderr, "Nachos: EOF detected in SynchConsole!\n");

    // Delete to test destructor
    delete sc;
}
