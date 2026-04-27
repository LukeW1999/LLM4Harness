#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list old_list = list;

    // Assume the list is not empty for the success path
    __CPROVER_assume(!aws_linked_list_empty(&list));

    // Ensure the list has at least one node
    __CPROVER_assume(old_list.head.next != &old_list.tail);

    struct aws_linked_list_node *front_node = aws_linked_list_pop_front(&list);

    // Assertions for success path
    if (front_node != NULL) {
        assert(list.head.next == old_list.head.next->next);
        assert(list.tail.prev == (aws_linked_list_empty(&list) ? &list.head : old_list.tail.prev));
        assert(front_node->next == NULL);
        assert(front_node->prev == NULL);
    } else {
        // Assertions for failure path
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
    }

    // Frame conditions
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
