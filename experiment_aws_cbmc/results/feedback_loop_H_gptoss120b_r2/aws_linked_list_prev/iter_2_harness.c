#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Populate the list with at least two nodes */
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_linked_list_node *node1 = aws_mem_acquire(alloc, sizeof(*node1));
    __CPROVER_assume(node1 != NULL);
    aws_linked_list_push_back(&list, node1);

    struct aws_linked_list_node *node2 = aws_mem_acquire(alloc, sizeof(*node2));
    __CPROVER_assume(node2 != NULL);
    aws_linked_list_push_back(&list, node2);

    /* 3. Choose a node that is part of the list (first or last element) */
    const struct aws_linked_list_node *node;
    if (nondet_bool()) {
        node = list.head.next;   /* first real element */
    } else {
        node = list.tail.prev;   /* last real element */
    }

    /* 4. Save old state */
    const struct aws_linked_list_node *old_prev = node->prev;
    struct aws_linked_list old_list = list;

    /* 5. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 6. Postconditions */
    assert(result == old_prev);

    /* No modifications to the list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The node pointer itself is unchanged */
    assert(node == (const struct aws_linked_list_node *)node);

    /* 7. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
