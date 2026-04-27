#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    struct aws_linked_list_node next_node;

    // Initialize the nodes
    after.next = &next_node;
    after.prev = NULL;
    next_node.prev = &after;
    next_node.next = NULL;
    to_add.next = NULL;
    to_add.prev = NULL;

    // Save old state
    struct aws_linked_list_node old_after = after;
    struct aws_linked_list_node old_to_add = to_add;

    int result = aws_linked_list_insert_after(&after, &to_add);

    if (result == AWS_OP_SUCCESS) {
        // Assert frame conditions and validity invariants for success path
        // after.next should be changed to to_add
        assert(after.next == &to_add);
        // after.next->prev should be changed to to_add
        assert(after.next->prev == &to_add);
        // to_add.prev should be changed to after
        assert(to_add.prev == &after);
        // to_add.next should be changed to old_after.next
        assert(to_add.next == old_after.next);
        // old_after.next->prev should be changed to to_add
        assert(old_after.next->prev == &to_add);
    } else {
        // Assert frame conditions and validity invariants for failure path
        // after.next should be unchanged
        assert(after.next == old_after.next);
        // after.next->prev should be unchanged
        assert(after.next->prev == &after);
        // to_add.prev should be unchanged
        assert(to_add.prev == old_to_add.prev);
        // to_add.next should be unchanged
        assert(to_add.next == old_to_add.next);
    }

    // Validity invariants
    assert(aws_linked_list_node_is_valid(&after));
    assert(aws_linked_list_node_is_valid(&to_add));
    assert(aws_linked_list_node_is_valid(&next_node));
}
