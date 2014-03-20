#pragma once

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#define TRUE	1
#define FALSE	0
typedef void			VOID;
typedef unsigned long	ULONG;
typedef unsigned char	BOOLEAN;
typedef struct _CACHE_BLOCK
{
	//...
	ULONG					Value;
}CACHE_BLOCK, *PCACHE_BLOCK;
/*************************************************/

#define QUEUE_DAT_T PCACHE_BLOCK

typedef struct _Queue
{
	ULONG				Size;
	ULONG				Head;
	ULONG				Tail;
	QUEUE_DAT_T			Data[];
}Queue, *PQueue;

VOID InitQueue (PQueue Queue, ULONG Size);

VOID DestroyQueue (PQueue Queue);

QUEUE_DAT_T* QueueRemove (PQueue Queue);

BOOLEAN QueueInsert(PQueue Queue, QUEUE_DAT_T* Entry);

ULONG QueueUsed (PQueue Queue);

BOOLEAN QueueIsFull (PQueue Queue);
