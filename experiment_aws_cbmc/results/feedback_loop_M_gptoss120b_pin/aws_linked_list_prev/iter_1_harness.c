#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is part of the list (or a sentinel) */
    const struct aws_linked_list_node *node;
    /* nondet choice: either head sentinel, tail sentinel, or a real node */
    bool choose_head = nondet_bool();
    bool choose_tail = nondet_bool();
    __CPROVER_assume(!(choose_head && choose_tail)); /* cannot be both */

    if (choose_head) {
        node = &list.head;
    } else if (choose_tail) {
        node = &list.tail;
    } else {
        /* pick a real node if the list is non‑empty, otherwise fall back to head */
        if (!aws_linked_list_empty(&list)) {
            node = list.head.next; /* first real node */
        } else {
            node = &list.head;
        }
    }
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof *node));

    /* 3. Save old state for immutability checks */
    struct aws_linked_list old_list = list;
    const struct aws_linked_list_node *old_prev = node->prev;

    /* 4. Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Post‑conditions */
    /* The function must return the previous pointer unchanged */
    assert(result == old_prev);

    /* The list must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 6. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
