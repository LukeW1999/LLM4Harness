#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    // Test 1: Single-node list
    {
        struct aws_linked_list list;
        struct aws_linked_list_node node;
        node.next = NULL;
        node.prev = NULL;

        list.head = &node;
        list.tail = &node;

        struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

        // Postcondition: popped node is the original head
        assert(popped == &node);
        // List should now be empty
        assert(list.head == NULL);
        assert(list.tail == NULL);
        // Popped node is removed (next/prev may be set to NULL)
        assert(popped->next == NULL);
        assert(popped->prev == NULL);
        assert(!aws_linked_list_node_is_in_list(popped));
    }

    // Test 2: Multi-node list (2 nodes)
    {
        struct aws_linked_list list;
        struct aws_linked_list_node nodeA, nodeB;
        nodeA.next = &nodeB;
        nodeA.prev = NULL;
        nodeB.next = NULL;
        nodeB.prev = &nodeA;

        list.head = &nodeA;
        list.tail = &nodeB;

        struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

        // Postcondition: popped is nodeA
        assert(popped == &nodeA);
        // New head is nodeB
        assert(list.head == &nodeB);
        // Tail remains nodeB
        assert(list.tail == &nodeB);
        // Popped node is removed (next/prev set to NULL)
        assert(popped->next == NULL);
        assert(popped->prev == NULL);
        // New head's prev is NULL
        assert(list.head->prev == NULL);
        assert(!aws_linked_list_node_is_in_list(popped));
    }
}
