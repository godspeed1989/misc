#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Single linked list
typedef struct node
{
    struct node * next;
} node;
typedef int (* remove_fn)(node const * v);
#define TRUE    1
#define FALSE   0

// Remove node(s) from the supplied list: for which the
// supplied remove function returns true.
// Returns the new head of the list.

// Not good at usage of pointers
node * remove_if1(node * head, remove_fn rmfn)
{
    node *curr, *prev = NULL, *next;
    curr = head;
    while (curr)
    {
        next = curr->next;
        if (rmfn(curr) == TRUE)
        {
            if (prev != NULL)
                prev->next = next;
            else
                head = next; // no previous, next is head now
            free(curr);
        }
        else
        {
            prev = curr;
        }
        curr = next;
    }
    return head;
}

// Good at usage of pointers
node* remove_if2(node * head, remove_fn rmfn)
{
    node **curr, *entry;
    for (curr = &head; *curr; )
    {
        entry = *curr;
        if (rmfn(entry))
        {
            *curr = entry->next;
            free(entry);
        }
        else
            curr = &entry->next;
    }
    return head;
}

#if 1
#define NUM 9999999

int remove_func(node const * v)
{
    return rand() & 1;
}

int main()
{
    int i;
    node *head, *curr;
    head = curr = (node*)malloc(sizeof(node));
    for (i = 1; i < NUM; i++)
    {
        curr->next = (node*)malloc(sizeof(node));
        curr = curr->next;
    }
    curr->next = NULL;

    srand(time(NULL));
    while (head)
    {
        if (rand() & 1)
            head = remove_if1(head, remove_func);
        else
            head = remove_if2(head, remove_func);
    }
    return 0;
}

#endif
