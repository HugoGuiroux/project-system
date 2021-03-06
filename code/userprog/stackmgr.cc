// stackmgr.cc
//      Routines to manage user stacks
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "stackmgr.h"
#include "system.h"
#include "addrspace.h"

/**
 * data_end_at indicate where the data segment end.
 * Suppose code & segment contiguous and code start at 0x0
 **/
StackMgr::StackMgr(AddrSpace *_space, unsigned int data_end_at)
{
    unsigned int total_memory_size;
    unsigned int total_free_size;
    unsigned int real_page_size = UserStackSize + PROTECTION_PAGE * PageSize;

    // Total memory size
    total_memory_size = NUM_VIRTUAL_PAGES * PageSize;
    DEBUG('t', "Total memory size : %lu (%lu pages)\n", total_memory_size, total_memory_size / PageSize);

    // Total memory allowed for stack
    total_free_size = total_memory_size - data_end_at;

    // Assert CONSTANT correctness
    ASSERT(STACK_PAGES_NUMBER < total_free_size / PageSize);

    total_free_size = STACK_PAGES_NUMBER * PageSize;
    DEBUG('t', "Total memory size for stack: %lu (%lu pages)\n", total_free_size, total_free_size / PageSize);

    // Compute number of possible stacks
    number_of_stack = total_free_size / real_page_size;
    DEBUG('t', "Number of possible stacks: %lu\n", number_of_stack);

    // Create bitmap
    bitmap = new BitMap(number_of_stack);

    // Keep first stack addr
    first_stack_addr = total_memory_size;
    DEBUG('t', "Code+Data ends at: %lu\n", data_end_at);

    // Get reference to space for virtual memory
    this->space = _space;
}

StackMgr::~StackMgr()
{
    // De-alocate bitmap
    delete bitmap;
}

/**
 * GetNewStack return address in user space corresponding to user stack, NULL if
 * no more memory
 **/
unsigned int StackMgr::GetNewStack()
{
    int index;
    unsigned int stack_addr;
    unsigned int real_page_size = UserStackSize + PROTECTION_PAGE * PageSize;

    // Find the first bit which is clear
    index = bitmap->FindFirst();

    DEBUG('t', "Return stack index %d", index);
    // If error, return NULL
    if (index == -1)
        return 0;

    // Compute stack addr
    stack_addr = first_stack_addr - index * real_page_size;

    DEBUG('t', "at addr %u\n", stack_addr);

    // Ask for page inside addr space
    // Don't ask protection page. Will trigger page fault
    space->AllocatePages(stack_addr - UserStackSize, UserStackSize / PageSize);

    return stack_addr;
}

/**
 * FreeStack
 * address : int corresponding to the stack freed
 * return -1 if invalid addr
 **/
int StackMgr::FreeStack(unsigned int addr)
{
    unsigned int stack_index;
    unsigned int real_page_size = UserStackSize + PROTECTION_PAGE * PageSize;

    // Check if addr does not goes out memory
    if (addr > first_stack_addr &&
        addr < (first_stack_addr - (number_of_stack - 1) * real_page_size))
        return -1;

    // Check if it is the begin of a stack
    if ((first_stack_addr - addr) % real_page_size != 0)
        return -1;

    // Compute stack index
    stack_index = (first_stack_addr - addr) / real_page_size;

    // Check if stack is used
    if (!bitmap->Test(stack_index))
        return -1;

    // Clean stack
    bitmap->Clear(stack_index);

    // Free pages inside @ space
    space->FreePages(addr - UserStackSize, UserStackSize / PageSize);

    return 0;
}

