#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);

    /* 2. Non‑deterministically decide how many nodes to add (bounded) */
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 3. Allocate nodes and push them onto the list */
    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *n = malloc(sizeof *n);
        __CPROVER_assume(n != NULL);
        /* nodes start with undefined links; push_back will set them */
        aws_linked_list_push_back(&list, n);
    }

    /* 4. Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Choose a node to query – the first node after the head sentinel */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL); /* always true for a properly initialized list */

    /* 6. Save old state for frame conditions */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_node = node;
    struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list_node *old_next = node->next;

    /* 7. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 8. Post‑conditions */

    /* 8.1. Return value must be the previous pointer */
    assert(result == old_prev);

    /* 8.2. No fields of the list may have changed */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 8.3. The queried node must be unchanged */
    assert(node == old_node);
    assert(node->prev == old_prev);
    assert(node->next == old_next);

    /* 8.4. The overall list must remain valid */
    assert(aws_linked_list_is_valid(&list));
}
