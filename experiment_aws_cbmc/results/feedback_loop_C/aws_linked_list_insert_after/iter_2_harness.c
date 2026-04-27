#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    struct aws_linked_list_node *old_after_next = after.next;
    struct aws_linked_list_node *old_to_add_prev = to_add.prev;
    struct aws_linked_list_node *old_to_add_next = to_add.next;

    // Initialize nodes
    after.next = NULL;
    after.prev = NULL;
    to_add.next = NULL;
    to_add.prev = NULL;

    // Simulate a valid list
    struct aws_linked_list list;
    list.head = &after;
    list.tail = &after;
    list.length = 1;

    int result = aws_linked_list_insert_after(&list, &after, &to_add);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(after.next == &to_add);
        assert(to_add.prev == &after);
        assert(to_add.next == old_after_next);
        if (old_after_next != NULL) {
            assert(old_after_next->prev == &to_add);
        }
    } else {
        // Failure path assertions
        assert(after.next == old_after_next);
        assert(to_add.prev == old_to_add_prev);
        assert(to_add.next == old_to_add_next);
    }

    // Validity invariants
    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
}
