#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_linked_list_next */
void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state for unchanged‑field checks */
    struct aws_linked_list old_list = list;

    /* 3. Choose a node from the list (could be any node, including sentinels) */
    struct aws_linked_list_node *node = list.head.next;

    /* Save the node's original links */
    struct aws_linked_list_node *old_next = node->next;
    struct aws_linked_list_node *old_prev = node->prev;

    /* 4. Expected result */
    struct aws_linked_list_node *expected = node->next;

    /* 5. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 6. Post‑condition: result must be the original next pointer */
    assert(result == expected);

    /* 7. Unchanged fields: the list structure itself must be unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 8. Unchanged fields: the node's links must be unchanged */
    assert(node->next == old_next);
    assert(node->prev == old_prev);

    /* 9. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
