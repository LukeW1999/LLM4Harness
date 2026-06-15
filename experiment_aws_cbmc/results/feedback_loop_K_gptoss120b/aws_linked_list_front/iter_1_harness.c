/*  
Preconditions:  
- `list` is a non‑null pointer.  
- `aws_linked_list_is_valid(list)` holds (the list satisfies all double‑linked‑list invariants).  

Postconditions (validity):  
- The returned pointer `r` is equal to `list->head.next`.  
- If the list is empty, `list->head.next` points to `&list->tail`, so `r` is the tail sentinel.  
- The list remains valid after the call (`aws_linked_list_is_valid(list)` still holds).  

Postconditions (frame):  
- No memory locations other than the returned pointer are modified; the `head` and `tail` nodes of the list retain their original `next` and `prev` pointers.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 5

void aws_linked_list_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* nondet number of nodes to populate the list */
    size_t n;
    __CPROVER_assume(n <= MAX_NODES);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* snapshot the list's structural pointers */
    struct aws_linked_list_node *head_next = list.head.next;
    struct aws_linked_list_node *head_prev = list.head.prev;
    struct aws_linked_list_node *tail_next = list.tail.next;
    struct aws_linked_list_node *tail_prev = list.tail.prev;

    /* call the function under test */
    struct aws_linked_list_node *r = aws_linked_list_front(&list);

    /* post‑condition: returned pointer is the first element */
    assert(r == head_next);

    /* post‑condition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* frame condition: head and tail nodes unchanged */
    assert(list.head.next == head_next);
    assert(list.head.prev == head_prev);
    assert(list.tail.next == tail_next);
    assert(list.tail.prev == tail_prev);

    return 0;
}
