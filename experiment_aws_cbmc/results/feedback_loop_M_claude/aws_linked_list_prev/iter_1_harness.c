#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_prev_harness() {
    /* 1. Allocate a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Pick a node to call prev on.
     *    We need a node that has a valid prev pointer.
     *    We'll use a node that is in the list (not the head sentinel,
     *    since head has no valid prev in the list sense, but tail does).
     *    Actually, any node with a non-NULL prev is valid.
     *    We'll use list.tail which always has a prev pointer set. */
    struct aws_linked_list_node *node = &list.tail;

    /* 3. Save old state */
    struct aws_linked_list_node *expected_prev = node->prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 5. Assert postconditions */
    /* The function returns node->prev */
    assert(result == expected_prev);

    /* The node itself is unchanged */
    assert(node->prev == expected_prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
