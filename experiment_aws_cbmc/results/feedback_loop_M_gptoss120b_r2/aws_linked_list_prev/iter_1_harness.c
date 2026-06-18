#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Choose a node that is part of the list */
    struct aws_linked_list_node *node = list.tail.prev;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(node));

    struct aws_linked_list_node *old_prev = node->prev;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Assert postconditions */
    /* Changed field: the returned pointer must be the previous node */
    assert(result == old_prev);

    /* Unchanged fields: the list must remain identical */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 6. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
