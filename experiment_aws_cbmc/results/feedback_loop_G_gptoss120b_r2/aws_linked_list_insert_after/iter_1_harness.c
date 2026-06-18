#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node that will be in the list (after) */
    struct aws_linked_list_node after_node;
    aws_linked_list_node_reset(&after_node);
    aws_linked_list_push_back(&list, &after_node);

    /* 3. Create a node that will be inserted (to_add) */
    struct aws_linked_list_node to_add_node;
    aws_linked_list_node_reset(&to_add_node);

    /* 4. Save old connections for post‑condition checks */
    struct aws_linked_list_node *old_after_next = after_node.next;   /* should be &list.tail */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;    /* should be &after_node */

    /* 5. Call the function under test */
    aws_linked_list_insert_after(&after_node, &to_add_node);

    /* 6. Post‑condition assertions */

    /* Connections around the inserted node */
    assert(after_node.next == &to_add_node);
    assert(to_add_node.prev == &after_node);
    assert(to_add_node.next == old_after_next);
    assert(old_after_next->prev == &to_add_node);

    /* List sentinel connections */
    assert(list.head.next == &after_node);
    assert(list.tail.prev == &to_add_node);

    /* Unchanged connections of the original node */
    assert(after_node.prev == &list.head);

    /* The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(&to_add_node));

    /* Global invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
