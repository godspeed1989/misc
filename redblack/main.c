#include "redblack.h"

#define MAX 100000

int main()
{
	int i;
	rb_tree_t tree;
	rb_node_t *node;
	rb_tree_create(&tree, NULL);

	for (i = 0; i < MAX; i++)
		rb_insert(&tree, i, NULL);
	rb_clear(&tree);
	for (i = 0; i < MAX; i+=2)
		rb_insert(&tree, i, NULL);
	for (i = 0; i < MAX; i+=4)
	{
		node = rb_find(&tree, i);
		rb_delete(&tree, node, FALSE);
	}

	rb_tree_destroy(&tree);
	return 0;
}
