#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* Declare and initialize list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Declare node (local variable) */
    struct aws_linked_list_node node;

    /* Save old state of list (shallow copy) */
    struct aws_linked_list old_list = list;

    /* Call function */
    aws_linked_list_push_back(&list, &node);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);

    /* Additional assertions from insert_before postconditions */
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));

    /* Unchanged fields: head and tail pointers themselves are same addresses */
    /* The list struct's head and tail are embedded, so their addresses are same */
    /* But we can assert that head.prev is still NULL (valid list invariant) */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* The node's next should be &list.tail */
    assert(node.next == &list.tail);
    /* The node's prev should be the old tail.prev */
    assert(node.prev == old_list.tail.prev);
}
