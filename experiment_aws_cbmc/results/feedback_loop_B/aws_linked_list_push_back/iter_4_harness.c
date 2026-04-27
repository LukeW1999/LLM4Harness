#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail = list.tail;
    struct aws_linked_list_node *old_last = list.tail ? list.tail->prev : NULL;

    // Handle both empty and non-empty list cases
    if (old_last == NULL) {
        // List is empty
        aws_linked_list_push_back(&list, &node);

        // Postconditions for empty list
        assert(list.head == &node);
        assert(list.tail == &node);
        assert(node.prev == NULL);
        assert(node.next == NULL);
    } else {
        // List is not empty
        aws_linked_list_push_back(&list, &node);

        // Postconditions for non-empty list
        assert(list.tail->prev == &node);
        assert(node.prev == old_last);
        assert(node.next == NULL);
        assert(old_last->next == &node);
        assert(list.head->prev == NULL);

        // Unchanged fields
        assert(list.head->next == old_list.head->next);
        assert(list.tail->prev->next == &list.tail);
        assert(list.head->prev == old_list.head->prev);
    }

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(&node));
    assert(aws_linked_list_node_prev_is_valid(&node));
    if (old_last != NULL) {
        assert(aws_linked_list_node_next_is_valid(old_last));
        assert(aws_linked_list_node_prev_is_valid(old_last));
    }
}
