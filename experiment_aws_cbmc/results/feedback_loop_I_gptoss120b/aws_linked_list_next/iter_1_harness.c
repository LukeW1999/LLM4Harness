#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Choose a node to query – use the head sentinel (always valid) */
    const struct aws_linked_list_node *node = &list.head;
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list old = list;
    const struct aws_linked_list_node *old_next = node->next;
    const struct aws_linked_list_node *old_prev = node->prev;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑conditions: result must be the original next pointer */
    assert(result == old_next);

    /* 6. Unchanged fields of the list */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 7. Unchanged fields of the queried node */
    assert(node->next == old_next);
    assert(node->prev == old_prev);

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
