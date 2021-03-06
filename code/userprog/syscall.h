/* syscalls.h
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel.
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */

#define SC_Halt                 0
#define SC_Exit                 1
#define SC_Exec                 2
#define SC_Join                 3
#define SC_Create               4
#define SC_Open                 5
#define SC_Read                 6
#define SC_Write                7
#define SC_Close                8
#define SC_ForkExec             9
#define SC_Yield                10
#define SC_PutChar              11
#define SC_GetChar              12
#define SC_PutString            13
#define SC_GetString            14
#define SC_GetInt               15
#define SC_PutInt               16
#define SC_UserThreadCreate     17
#define SC_UserThreadExit       18
#define SC_UserThreadJoin       19
#define SC_UserSemaphoreCreate	20
#define SC_UserSemaphoreP       21
#define SC_UserSemaphoreV       22
#define SC_UserSemaphoreDestroy	23
#define SC_AllocPageHeap        24
#define SC_FreePageHeap         25
#define SC_Waitpid              26
#define SC_CheckEnd             27
#define SC_Seek                 28
#define SC_GetCurrentDirectory  29
#define SC_SetCurrentDirectory  30
#define SC_Remove               31
#define SC_Listen				32
#define SC_Accept				33
#define SC_Connect				34
#define SC_Send					35
#define SC_Receive				36
#define SC_Disconnect			37

#ifdef IN_USER_MODE

// LB: This part is read only on compiling the test/*.c files.
// It is *not* read on compiling test/start.S


/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking,
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt () __attribute__((noreturn));


/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit (int status) __attribute__((noreturn));

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the
 * address space identifier
 */
SpaceId Exec (char *name);

/* Only return once the the user program "id" has finished.
 * Return the exit status.
 */
int Join (SpaceId id);


/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */

/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;

/* when an address space starts up, it has two open files, representing
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0
#define ConsoleOutput	1

/* Create a Nachos file, with "name" */
int Create (char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can
 * be used to read and write to the file.
 */
OpenFileId Open(char *name);

/* Write "size" bytes from "buffer" to the open file. */
int Write(OpenFileId id, char *buffer, int size);

/* Read "size" bytes from the open file into "buffer".
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough
 * characters to read, return whatever is available (for I/O devices,
 * you should always wait until you can return at least one character).
 */
int Read(OpenFileId id, char *buffer, int size);

/* Close the file, we're done reading and writing to it. */
int Close(OpenFileId id);

/* Seek into the file at position pos */
int Seek(OpenFileId id, int pos);

/* Unlink/Remove file */
int Remove(const char *name);



/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program.
 */

/* Fork a thread to run a procedure ("func") in the *same* address space
 * as the current thread.
 */
//void Fork (void (*func) ()); //the original definition was for threads
int Fork();


/* Yield the CPU to another runnable thread, whether in this address space
 * or not.
 */
void Yield ();

/* PutChar write the character c to the console. */
void PutChar(char c);

/* GetChar get a character from the console. 
 * Return the character or EOF if no more character to read.
 */
int GetChar();

/* Putstring reads at most n character in the console. return s if there is no error 
 * otherwise it will return NULL if there is error or EOF.
 */
void PutString(char s[]);

/* GetString writes the string s to the console. If the string given is longer
 * than MAX_STRING_SIZE then the remaining part is not printed in the console.
 */
char* GetString(char *s, int n);

/* PutInt is the function that use to write the int i to the console */
void PutInt(int i);

/* GetInt is the function that read an int from the input buffer to the p pointer adress.
 * Return 0 when everything is ok, -1 when the input cannot be read as an int
 * and -2 when the adress p cannot be wroten by the caller.
 */
int GetInt();

//Thread managing functions (see user_doc.org)
int UserThreadCreate(void *(*f) (void *arg), void *arg);
void UserThreadExit(void *ret);
int UserThreadJoin(int tid, void **retval);

//Semaphore managing functions (see user_doc.org)
int UserSemaphoreCreate(char* name, int value); //create semaphore in the user level
int UserSemaphoreP(int id); 			//Semaphore to lock the thread
int UserSemaphoreV(int id); 			//Semaphore to unlock the thread
int UserSemaphoreDestroy(int id);		//To destroy the semaphore syscall

/* Run the executable, in the thread , and return the int 
 * and has arg s as program you want to execute 
 */
int ForkExec(char *s);

/**
 * AllocPageHeap asks for a new page on heap
 * return -1 if no more page for heap, new page addr otherwise
 **/
int AllocPageHeap();

/**
 * FreePageHeap gives back a new page for heap
 **/
int FreePageHeap();

/**
 * Waitpid wait on the process which pid is given as argument
 */
int Waitpid(unsigned int pid, int *exitcode);

/**
 * CheckEnd checking of the process if it is end or not 
 */
int CheckEnd(unsigned int pid);

//-------------------------------------------------------------//
//NETWORK SYSCALLS

/**
 * Listen create a listening socket on the port passed in argument.
 * return the sid of the created socket
 * return -1 if the port does not exists
 * return -2 if the port is already listening in another socket
 */
int Listen(int local_port);

/**
 * Accept take a listening socket. It block until a client use connect to this listening socket, 
 * then a connection is established and accept return the socket created.
 * Note: the listening socket is still listening after a call to accept.
 * return the sid of the created socket
 * return -1 if listening_socket is not a listening socket
 * return -2 if there is no more available socket slot in the port (limited to NB_CONNECTION_PER_PORT)
 * return -3 if the same socket already exists
 * return -4 if the connection is timed out
*/ 
int Accept(int listening_socket);

/**
 * Connect try to establish a connection to the machine and port passed in argument.
 * The aimed port should be listening.
 * return the sid of the created socket
 * return -1 there is not any place to create the new socket
 * return -2 the connection is timed out
*/
int Connect(int remote_machine, int remote_port);

/**
 * Send send size bytes from the buffer in socket
 * return 0 if the data are well send
 * return -1 if the socket is not connected
 * return -2 if the sending is timed out
 * return -3 if the socket has been closed in the other side
 * Note: if the socket is closed in the other side, we can still read the buffered data in the socket.
*/
int Send(int socket, char* buffer, unsigned int size);

/**
 * Receive read at most size bytes from the socket to the buffer.
 * If blocking is true, receive wait for size bytes (there is a timeout).
 * If blocking is false, receive read directly as many bytes as it can.
 * return the number of read bytes
 * return -1 if timed out
*/
int Receive(int socket, char* buffer, unsigned int size, char blocking);

/**
* Disconnect close the socket
*/
void Disconnect(int socket);
//-------------------------------------------------------------//

/**
 * Get the current directory of a process
 **/
char *GetCurrentDirectory();

/**
 * Set the current directory of a process
 **/
int SetCurrentDirectory(char *name);
#endif // IN_USER_MODE

#endif /* SYSCALL_H */
