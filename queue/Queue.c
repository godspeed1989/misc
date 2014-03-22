#include "Queue.h"

#define TEST

#define _QMALLOC(n)			malloc(n)
#define _QFREE(p)			free(p)
#define _QZEROMEM(p,len)	memset(p,0,len)

BOOLEAN InitQueue (PQueue Queue, ULONG Size)
{
	_QZEROMEM(Queue, sizeof(Queue));
	Queue->Data = (QUEUE_DAT_T*)_QMALLOC(Size*sizeof(QUEUE_DAT_T));
	if (Queue->Data == NULL)
		return FALSE;
	Queue->Head = 0;
	Queue->Tail = 0;
	Queue->Used = 0;
	Queue->Size = Size;
	return TRUE;
}

VOID DestroyQueue (PQueue Queue)
{
	if (Queue->Data != NULL)
		_QFREE(Queue->Data);
	_QZEROMEM(Queue, sizeof(Queue));
}

BOOLEAN QueueInsert (PQueue Queue, QUEUE_DAT_T Entry)
{
	if (QueueIsFull(Queue) == TRUE)
		return FALSE;
	Queue->Data[Queue->Tail] = Entry;
	Queue->Tail = (Queue->Tail + 1) % Queue->Size;
	Queue->Used++;
	return TRUE;
}

QUEUE_DAT_T QueueRemove (PQueue Queue)
{
	QUEUE_DAT_T ret;
	if (QueueIsEmpty(Queue) == TRUE)
		return NULL;
	ret = Queue->Data[Queue->Head];
	Queue->Head = (Queue->Head + 1) % Queue->Size;
	Queue->Used--;
	return ret;
}

BOOLEAN QueueIsEmpty (PQueue Queue)
{
	return (Queue->Used == 0);
}

BOOLEAN QueueIsFull (PQueue Queue)
{
	return (Queue->Used == Queue->Size);
}

#ifdef TEST
#define QUEUE_SIZE 409600

int main()
{
	ULONG i, j;
	Queue Queue;
	PCACHE_BLOCK *blocks, pblock;

	blocks = (PCACHE_BLOCK*)malloc(QUEUE_SIZE*sizeof(PCACHE_BLOCK));
	assert(blocks);
	for (i = 0; i < QUEUE_SIZE; i++) {
		assert(blocks[i] = (PCACHE_BLOCK)malloc(sizeof(CACHE_BLOCK)));
		blocks[i]->Value = i;
	}
	assert(InitQueue(&Queue, QUEUE_SIZE/2));

	QueueInsert(&Queue, blocks[0]);
	QueueInsert(&Queue, blocks[1]);
	QueueRemove(&Queue);
	QueueRemove(&Queue);

	i = 0, j = 0;
	while (QueueInsert(&Queue, blocks[i++]) == TRUE);
	while ((pblock = QueueRemove(&Queue)) != NULL)
		assert(pblock->Value == blocks[j++]->Value);

	srand((unsigned int)time(NULL));
	i = 0, j = 0;
	while (i < QUEUE_SIZE)
	{
		if (rand() & 1)
			QueueInsert(&Queue, blocks[i++]);
		else if ((pblock = QueueRemove(&Queue)) != NULL)
			assert(pblock->Value == blocks[j++]->Value);
	}
	while ((pblock = QueueRemove(&Queue)) != NULL)
		assert(pblock->Value == blocks[j++]->Value);

	DestroyQueue(&Queue);
	for (i = 0; i < QUEUE_SIZE; i++)
		free(blocks[i]);
	free(blocks);
	return 0;
}

#endif
