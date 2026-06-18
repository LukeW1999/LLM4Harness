#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_rend_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state of the sentinel nodes to check immutability */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    /* Postcondition: returned pointer must be to the head sentinel */
    assert(rval != NULL);
    assert(rval == &list.head);

    /* The list itself is not modified by this function */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
