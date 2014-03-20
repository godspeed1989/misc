#include "Queue.h"

#define TEST

VOID InitQueue (PQueue Queue, ULONG Size)
{
	Queue->Size = Size;
}

VOID DestroyQueue (PQueue Queue)
{
	Queue->Size = 0;
}

QUEUE_DAT_T* QueueRemove (PQueue Queue)
{
	return NULL;
}

BOOLEAN QueueInsert(PQueue Queue, QUEUE_DAT_T* Entry)
{
	return FALSE;
}

ULONG QueueUsed (PQueue Queue)
{
	return 0;
}

BOOLEAN QueueIsFull (PQueue Queue)
{
	return FALSE;
}

#ifdef TEST
#define QUEUE_SIZE 409600

int main()
{
	Queue Queue;

	return 0;
}

#endif
