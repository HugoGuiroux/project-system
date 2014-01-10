#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H
#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"


class SynchConsole {
  public:
    SynchConsole(char *readFile, char *writeFile);	// initialize the hardware console device
    ~SynchConsole();	// clean up console emulation
    void SynchPutChar(const char ch);	// Unix putchar(3S) 
    int SynchGetChar();	// Unix getchar(3S)
    void SynchPutString(const char *s);	// Unix puts(3S)
    char *SynchGetString(char *s, int n);	// Unix fgets(3S)
    void SynchPutInt(int i);
    int SynchGetInt();

  private:
    void __PutChar(const char ch);
    int __GetChar();
    Semaphore *monitor;
  private:
    Console *console;
};

#endif