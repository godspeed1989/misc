
typedef unsigned int u32;

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
		swap2(z[k], z[m]);
		heapify(z, n, m);
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
		heapify(z, n, j);
		--j;
	}
}

template <typename Type>
void heap_sort(Type *x, u32 n)
{
	build_heap(x, n);
	Type *p = x - 1;
	for (u32 k=n; k>1; --k)
	{
		swap2(p[1], p[k]); // move largest to end of array
		--n; // remaining array has one element less
		heapify(p, n, 1); // restore heap-property
	}
}

