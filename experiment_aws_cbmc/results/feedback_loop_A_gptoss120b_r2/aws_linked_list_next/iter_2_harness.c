#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list is non‑empty so that a valid interior node exists */
    __CPROVER_assume(list.head.next != &list.tail);
    __CPROVER_assume(AWS_MEM_IS_READABLE(list.head.next, sizeof *list.head.next));

    /* 3. Choose a node that is guaranteed to be a valid interior node (not the tail sentinel) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof *node));
    __CPROVER_assume(AWS_MEM_IS_READABLE(node->next, sizeof *node->next));
    __CPROVER_assume(AWS_MEM_IS_READABLE(node->prev, sizeof *node->prev));

    /* 4. Save old state */
    struct aws_linked_list_node old_node = *node;
    struct aws_linked_list old_list = list;

    /* 5. Call function under test */
    struct aws_linked_list_node *r = aws_linked_list_next(node);

    /* 6. Assert post‑conditions */
    assert(r == old_node.next);
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 7. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
