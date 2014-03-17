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
	struct _CACHE_BLOCK*	Prior;
	struct _CACHE_BLOCK*	Next;
}CACHE_BLOCK, *PCACHE_BLOCK;
/*************************************************/

#define LIST_DAT_T CACHE_BLOCK

typedef struct _List
{
	ULONG			Size;
	LIST_DAT_T*		Head;
	LIST_DAT_T*		Tail;
}List, *PList;

VOID InitList (PList List);

VOID DestroyList (PList List);

LIST_DAT_T* ListRemoveHead (PList List);

LIST_DAT_T* ListRemoveTail (PList List);

VOID ListInsertToHead (PList List, LIST_DAT_T* Entry);

VOID ListInsertToTail (PList List, LIST_DAT_T* Entry);

VOID ListInsertAfter (PList List, LIST_DAT_T* After, LIST_DAT_T* Entry);

VOID ListInsertBefore (PList List, LIST_DAT_T* Before, LIST_DAT_T* Entry);

VOID ListDelete (PList List, LIST_DAT_T* Entry);

VOID ListMoveToHead (PList List, LIST_DAT_T* Entry);

VOID ListMoveToTail (PList List, LIST_DAT_T* Entry);
