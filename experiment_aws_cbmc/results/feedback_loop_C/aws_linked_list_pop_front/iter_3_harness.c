#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list old_list;
    struct aws_linked_list_node old_node;

    // Initialize the list and node
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));
    node.next = NULL;
    node.prev = NULL;

    // Store the front node before popping
    struct aws_linked_list_node *front_node = aws_linked_list_front(&list);

    // Save the old state of the list
    old_list = list;
    old_node = *front_node;

    // Call the function
    aws_linked_list_pop_front(&list);

    // Success path assertions
    if (!aws_linked_list_empty(&old_list)) {
        assert(list.head.next == old_node.next);
        if (old_node.next != NULL) {
            assert(old_node.next->prev == &list.head);
        }
        assert(front_node->next == NULL);
        assert(front_node->prev == NULL);
    } else {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }

    // Frame condition assertions
    assert(list.tail.next == &list.tail);
    if (!aws_linked_list_empty(&list)) {
        assert(list.tail.prev != &old_list.head);
    }

    // Validity invariant assertions
    assert(aws_linked_list_is_valid(&list));
}
