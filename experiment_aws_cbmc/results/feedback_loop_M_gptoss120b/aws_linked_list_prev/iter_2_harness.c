#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_prev_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list contains at least one real node */
    __CPROVER_assume(list.head.next != &list.tail);
    __CPROVER_assume(list.head.next != NULL);

    /* 3. Choose a real node to query (first element) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(node));
    __CPROVER_assume(aws_linked_list_node_next_is_valid(node));

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = *node;

    /* 5. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_prev(node);

    /* 6. Post‑condition: return value equals the node's prev pointer */
    assert(result == old_node.prev);

    /* 7. Unchanged fields: list structure */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 8. Unchanged fields: the queried node */
    assert(node->next == old_node.next);
    assert(node->prev == old_node.prev);

    /* 9. Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
