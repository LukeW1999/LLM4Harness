#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    // Initialize the list and node with non-deterministic values
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    node.next = NULL;
    node.prev = NULL;

    // Save old state
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_node = node;

    // Call the function under test
    int result = aws_linked_list_push_back(&list, &node);

    // Assertions based on the analysis
    assert(aws_linked_list_is_valid(&list));

    // Success path assertions
    if (result == AWS_OP_SUCCESS) {
        assert(list.head.next == (old_list.head.next == &old_list.tail ? &node : old_list.head.next));
        assert(list.tail.prev == &node);
        assert(node.next == &list.tail);
        assert(node.prev == (old_list.head.next == &old_list.tail ? &list.head : old_list.tail.prev));
    }

    // Failure path assertions (these should not change the state)
    if (result == AWS_OP_ERR) {
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
        assert(node.next == old_node.next);
        assert(node.prev == old_node.prev);
    }

    // Frame conditions
    assert(list.head.prev == &list.head);
    assert(list.tail.next == &list.tail);
}
