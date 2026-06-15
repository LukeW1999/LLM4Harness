#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    // Initialize list with sentinel nodes
    struct aws_linked_list list;
    struct aws_linked_list_node head, tail;
    list.head = &head;
    list.tail = &tail;

    // Maximum number of data nodes
    struct aws_linked_list_node nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];

    // Non-deterministic number of nodes (0 to MAX)
    size_t num_nodes;
    __CPROVER_assume(num_nodes <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Build the linked list
    if (num_nodes == 0) {
        // Empty list: head and tail point to each other
        head.next = &tail;
        head.prev = NULL;
        tail.prev = &head;
        tail.next = NULL;
    } else {
        // Non-empty list: link nodes between head and tail
        head.next = &nodes[0];
        head.prev = NULL;
        for (size_t i = 0; i < num_nodes; i++) {
            nodes[i].prev = (i == 0) ? &head : &nodes[i-1];
            nodes[i].next = (i == num_nodes-1) ? &tail : &nodes[i+1];
        }
        tail.prev = &nodes[num_nodes-1];
        tail.next = NULL;
    }

    // Assume initial validity
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Save original state
    struct aws_linked_list old = list;

    // Call function under test
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    // Postcondition assertions
    if (num_nodes == 0) {
        // Empty list: back returns head
        assert(result == &head);
    } else {
        // Non-empty list: back returns tail.prev (last element)
        assert(result == tail.prev);
        assert(result != &head);
        assert(result != &tail);
        assert(result == &nodes[num_nodes-1]);
    }

    // List must not have been modified
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    // Validity must be preserved
    assert(aws_linked_list_is_valid(&list));
}
