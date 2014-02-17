#include "Heap.h"

#define Parent(i)	((i-1)/2)
#define Left(i)		(2*i+1)
#define Right(i)	(2*i+2)

#define TEST

#define _HMALLOC(n)	malloc(n)
#define _HFREE(p)	free(p)

BOOLEAN
InitHeap(PHeap Heap, ULONG Size)
{
	Heap->Used = 0;
	Heap->Size = Size;
	Heap->Entries = (PHeapEntry*)_HMALLOC(Size*sizeof(PHeapEntry));
	return (Heap->Entries != NULL);
}

VOID
DestroyHeap(PHeap Heap)
{
	ULONG i;
	for (i=0; i<Heap->Used; i++)
		_HFREE(Heap->Entries[i]);
	Heap->Used = 0;
	Heap->Size = 0;
	if (Heap->Entries != NULL)
	{
		_HFREE(Heap->Entries);
		Heap->Entries = NULL;
	}
}

static VOID
HeapSiftUp(PHeap Heap, ULONG HeapIndex)
{
	BOOLEAN done = FALSE;
	PHeapEntry* h = Heap->Entries;
	while (HeapIndex != 0 && done == FALSE)
	{
		ULONG ParentIndex = Parent(HeapIndex);
		if (HEAP_ENTRY_COMPARE(h, ParentIndex, HeapIndex))
		{
			HEAP_ENTRY_SWAP(h, ParentIndex, HeapIndex);
			HeapIndex = ParentIndex;
		}
		else
			done = TRUE;
	}
}

static VOID
HeapSiftDown(PHeap Heap, ULONG HeapIndex)
{
	BOOLEAN done = FALSE;
	PHeapEntry* h = Heap->Entries;
	while (Left(HeapIndex) < Heap->Used && done == FALSE)
	{
		ULONG Index = Left(HeapIndex);
		if (Right(HeapIndex) < Heap->Used &&
			!HEAP_ENTRY_COMPARE(h, Left(HeapIndex), Right(HeapIndex)))
			Index = Right(HeapIndex);
		if (HEAP_ENTRY_COMPARE(h, HeapIndex, Index))
		{
			HEAP_ENTRY_SWAP(h, HeapIndex, Index);
			HeapIndex = Index;
		}
		else
			done = TRUE;
	}
}

BOOLEAN
HeapInsert(PHeap Heap, HEAP_DAT_T *pData)
{
	PHeapEntry entry = NULL;
	if (Heap->Used == Heap->Size)
		return FALSE;
	entry = (PHeapEntry)_HMALLOC(sizeof(HeapEntry));
	if (entry == NULL)
		return FALSE;

#ifndef TEST
	entry->Value = 0;
#else
	entry->Value = rand();
#endif
	entry->pData = pData;
	pData->HeapIndex = Heap->Used;
	Heap->Used++;
	Heap->Entries[pData->HeapIndex] = entry;

	HeapSiftUp(Heap, pData->HeapIndex);
	return TRUE;
}

VOID
HeapDelete(PHeap Heap, ULONG HeapIndex)
{
	PHeapEntry* h = Heap->Entries;
	if (Heap->Used == 0)
		return;
	Heap->Used--;
	HEAP_ENTRY_SWAP(h, HeapIndex, Heap->Used);

	if (HEAP_ENTRY_COMPARE(h, HeapIndex, Heap->Used))
		HeapSiftUp(Heap, HeapIndex);
	else
		HeapSiftDown(Heap, HeapIndex);

	_HFREE(Heap->Entries[Heap->Used]);
	Heap->Entries[Heap->Used] = NULL;
}

VOID
HeapIncreaseValue(PHeap Heap, ULONG HeapIndex, ULONG Inc)
{
	Heap->Entries[HeapIndex]->Value += Inc;
#ifdef MIN_HEAP
	HeapSiftDown(Heap, HeapIndex);
#else
	HeapSiftUp(Heap, HeapIndex);
#endif
}

VOID
HeapDecreaseValue(PHeap Heap, ULONG HeapIndex, ULONG Dec)
{
	if (Heap->Entries[HeapIndex]->Value > Dec)
		Heap->Entries[HeapIndex]->Value -= Dec;
	else
		Heap->Entries[HeapIndex]->Value = 0;
#ifdef MIN_HEAP
	HeapSiftUp(Heap, HeapIndex);
#else
	HeapSiftDown(Heap, HeapIndex);
#endif
}

HEAP_DAT_T*
GetAndRemoveHeapTop(PHeap Heap)
{
	HEAP_DAT_T* ret;
	if (Heap->Used == 0)
		return NULL;
	ret = Heap->Entries[0]->pData;
	HeapDelete(Heap, 0);
	return ret;
}

#ifdef TEST
#define HEAP_SIZE 64
int main()
{
	ULONG i;
	Heap Heap;
	HEAP_DAT_T *HeapData;
	srand((unsigned int)time(NULL));

	HeapData = (HEAP_DAT_T*)malloc(HEAP_SIZE*sizeof(HEAP_DAT_T));
	if (HeapData==NULL || InitHeap(&Heap, HEAP_SIZE)==FALSE)
	{
		printf("Init Heap Failed\n");
		return 1;
	}

	i = 0;
	while (TRUE == HeapInsert(&Heap, HeapData+i))
		i++;

	/*for (i=0; i<HEAP_SIZE*2; i++)
	{
		HeapIncreaseValue(&Heap, rand()%HEAP_SIZE, rand());
		HeapDecreaseValue(&Heap, rand()%HEAP_SIZE, rand());
	}*/

	for (i=0; i<HEAP_SIZE; i++)
	{
		if (HeapData+i != Heap.Entries[HeapData[i].HeapIndex]->pData)
			printf("%p - %p Ptr Error\n", HeapData+i, Heap.Entries[HeapData[i].HeapIndex]->pData);
		if (i!=0 && HEAP_ENTRY_COMPARE(Heap.Entries, Parent(i), i))
			printf("%ld - %ld Order Error\n", Parent(i), i);
	}

	for (i=0; i<HEAP_SIZE; i++)
		HeapDelete(&Heap, HeapData[i].HeapIndex);
	DestroyHeap(&Heap);
	return 0;
}
#endif
