#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list_node after;
    struct aws_linked_list_node to_add;
    struct aws_linked_list_node dummy_node;
    struct aws_linked_list list;

    // Initialize nodes
    after.next = NULL;
    after.prev = NULL;
    to_add.next = NULL;
    to_add.prev = NULL;
    dummy_node.next = NULL;
    dummy_node.prev = NULL;

    // Simulate a valid list
    list.head = &after;
    list.tail = &after;
    list.length = 1;

    // Test inserting when after.next is NULL
    int result = aws_linked_list_insert_after(&list, &after, &to_add);
    assert(result == AWS_OP_SUCCESS);
    assert(after.next == &to_add);
    assert(to_add.prev == &after);
    assert(to_add.next == NULL);
    assert(list.tail == &to_add);
    assert(list.length == 2);

    // Reset list and nodes for next test
    after.next = &dummy_node;
    after.prev = NULL;
    to_add.next = NULL;
    to_add.prev = NULL;
    dummy_node.next = NULL;
    dummy_node.prev = &after;
    list.head = &after;
    list.tail = &dummy_node;
    list.length = 2;

    // Test inserting when after.next is not NULL
    result = aws_linked_list_insert_after(&list, &after, &to_add);
    assert(result == AWS_OP_SUCCESS);
    assert(after.next == &to_add);
    assert(to_add.prev == &after);
    assert(to_add.next == &dummy_node);
    assert(dummy_node.prev == &to_add);
    assert(list.tail == &dummy_node);
    assert(list.length == 3);

    // Validity invariants
    assert(aws_linked_list_node_next_is_valid(&after));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&dummy_node));
    assert(aws_linked_list_node_prev_is_valid(&dummy_node));
}
