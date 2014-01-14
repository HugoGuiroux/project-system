#include "syscall.h"

void fun(void* arg)
{
	int i;
	for(i=0;i<10000;i++);
	PutString("Child finish\n");
	UserThreadExit();
}

int main()
{
	PutString("Parent start\n");
	UserThreadJoin(UserThreadCreate(&fun, 0));
	PutString("Parent finish\n");
	return 0;
}
