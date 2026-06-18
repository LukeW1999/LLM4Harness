#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list old_list = list;

    /* 3. Prepare the new node (not already in the list) */
    struct aws_linked_list_node node;
    /* Assume the node is not linked anywhere (typical precondition) */
    __CPROVER_assume(node.next == NULL && node.prev == NULL);

    /* 4. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Assert postconditions */

    /* 5a. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
    /* tail sentinel's prev is valid (from insert_before) */
    assert(aws_linked_list_node_prev_is_valid(&list.tail));

    /* 5b. Changed fields (from Doxygen) */
    assert(list.tail.prev == &node); /* node is the new last element */

    /* 5c. Explicit pointer linkage */
    assert(node.next == &list.tail);
    assert(node.prev == old_list.tail.prev);
    assert(old_list.tail.prev->next == &node);

    /* 5d. Unchanged sentinel fields */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 5e. Head.next changes only if list was empty */
    if (old_list.head.next == &old_list.tail) {
        assert(list.head.next == &node);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    /* 5f. Other nodes (if any) remain unchanged – implicitly checked by validity */
}
