#include <iostream>
#include <cstdlib>

typedef unsigned int u32;

template <typename Type>
void swap2(Type &a, Type &b)
{
	Type t;
	t = a;
	a = b;
	b = t;
}

template <typename Type>
void heapify(Type *z, u32 n, u32 k)
// Data expected in z[1,2,...,n].
{
	u32 m = k; // index of max of k, left(k), and right(k)

	const u32 l = (k<<1); // left(k);
	if ( (l <= n) && (z[l] > z[k]) ) m = l; // left child (exists and) greater than k

	const u32 r = (k<<1) + 1; // right(k);
	if ( (r <= n) && (z[r] > z[m]) ) m = r; // right child (ex. and) greater than max(k,l)

	if ( m != k ) // need to swap
	{
		swap2<Type>(z[k], z[m]);
		heapify<Type>(z, n, m);
	}
}

template <typename Type>
void build_heap(Type *x, u32 n)
// Reorder data to a heap.
// Data expected in x[0,1,...,n-1].
{
	Type *z = x - 1; // make one-based
	u32 j = (n>>1); // max index such that node has at least one child
	while ( j > 0 )
	{
		heapify<Type>(z, n, j);
		--j;
	}
}

template <typename Type>
void heap_sort(Type *x, u32 n)
{
	build_heap<Type>(x, n);
	Type *p = x - 1;
	for (u32 k=n; k>1; --k)
	{
		swap2<Type>(p[1], p[k]); // move largest to end of array
		--n; // remaining array has one element less
		heapify<Type>(p, n, 1); // restore heap-property
	}
}

#define NUM 1024

int main()
{
	u32 a[NUM];
	for(;;)
	{
		for(u32 i = 1; i < NUM; ++i)
			a[i] = (u32)rand();
		heap_sort<u32>(a, NUM);
		for(u32 i = 1; i < NUM; ++i)
		{
			if(a[i] < a[i-1])
				throw;
		}
	}
	return 0;
}

