#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    /* Data structure */
    struct aws_linked_list list;
    struct aws_linked_list old_list;

    /* Non-deterministically allocate and initialize the list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Assume the list is valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    old_list = list;

    /* Call the function */
    struct aws_linked_list_node *front = aws_linked_list_front((const struct aws_linked_list *)&list);

    /* Postconditions */
    /* Return value must be the first node in the list */
    assert(front == list.head.next);

    /* List structure must remain unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* The list remains valid */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Since list is non-empty, the front node must not be the tail sentinel */
    assert(front != &list.tail);
}
