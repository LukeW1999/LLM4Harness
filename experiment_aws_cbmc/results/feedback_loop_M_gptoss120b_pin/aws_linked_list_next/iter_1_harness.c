#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node to query – use the list head (always valid) */
    struct aws_linked_list_node *node = &list.head;

    /* 3. Save old state before the call */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Post‑conditions */

    /* 5a. The returned pointer must be the original next pointer */
    assert(result == old_node.next);

    /* 5b. The list structure must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5c. The queried node must remain unchanged */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 5d. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
