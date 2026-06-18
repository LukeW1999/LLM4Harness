#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* Initialize list with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Assume the new node is detached (not in any list) */
    __CPROVER_assume(node.next == NULL && node.prev == NULL);

    /* Save old last node (tail.prev) before insertion */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* Postconditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.tail.prev == &node);
    assert(node.next == &list.tail);
    assert(node.prev == old_last);
    assert(old_last->next == &node);

    /* Sentinel invariants remain */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
