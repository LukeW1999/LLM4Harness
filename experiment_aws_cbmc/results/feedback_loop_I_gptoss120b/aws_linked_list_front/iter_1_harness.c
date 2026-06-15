#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Harness for aws_linked_list_front */
void aws_linked_list_front_harness() {
    /* 1. Allocate and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* 4. Postconditions */

    /* 4a. The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 4b. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));

    /* 4c. Result pointer conditions */
    if (aws_linked_list_empty(&list)) {
        /* When the list is empty, front should be the tail sentinel */
        assert(result == &list.tail);
    } else {
        /* When not empty, result must be a node that belongs to the list */
        assert(result != &list.tail);
        assert(aws_linked_list_node_is_in_list(result));
    }
}
