#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 3. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 4. Assert postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);

    /* Node connectivity */
    assert(node.prev == old_tail_prev);
    assert(node.next == &list.tail);

    /* If list was empty, head.next should now point to node */
    if (aws_linked_list_empty(&old_list)) {
        assert(list.head.next == &node);
    } else {
        /* Otherwise, head.next remains unchanged */
        assert(list.head.next == old_head_next);
    }

    /* List struct fields that should not change (except head.next/tail.prev, which are updated) */
    /* head.prev is always NULL for valid list */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* Old tail.prev's next should now be node if list was non-empty */
    if (!aws_linked_list_empty(&old_list)) {
        assert(old_tail_prev->next == &node);
    }

    /* 5. Ensure node is in list */
    assert(aws_linked_list_node_is_in_list(&node));
}
