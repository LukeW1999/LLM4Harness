#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;
    struct aws_linked_list_node prev_node;

    // Initialize nodes
    before.next = NULL;
    before.prev = &prev_node;
    to_add.next = NULL;
    to_add.prev = NULL;
    prev_node.next = &before;
    prev_node.prev = NULL;

    // Save old state
    struct aws_linked_list_node before_old = before;
    struct aws_linked_list_node to_add_old = to_add;
    struct aws_linked_list_node prev_node_old = prev_node;

    // Call the function under test
    aws_linked_list_insert_before(&before, &to_add);

    // Assertions for success path
    assert(before.prev == &to_add);
    assert(prev_node.next == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == &prev_node);

    // Frame conditions
    assert(before.next == before_old.next);
    assert(prev_node.prev == prev_node_old.prev);

    // Validity invariants
    assert(aws_linked_list_node_is_valid(&before));
    assert(aws_linked_list_node_is_valid(&to_add));
    assert(aws_linked_list_node_is_valid(&prev_node));
}
