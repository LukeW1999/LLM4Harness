#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node1, node2;

    // Non-deterministically initialize nodes
    node1.next = nondet_bool() ? &node2 : NULL;
    node1.prev = nondet_bool() ? NULL : &node2;
    node2.next = nondet_bool() ? NULL : &node1;
    node2.prev = nondet_bool() ? &node1 : NULL;

    // Initialize list head
    list.head.next = &node1;
    list.head.prev = &node2;

    // Ensure list is non-empty and valid
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Save old state
    struct aws_linked_list old_list = list;

    // Call function under test
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    // Postconditions
    assert(popped != NULL); // list was non-empty
    assert(popped == old_list.head.next); // popped the first node
    assert(aws_linked_list_is_valid(&list)); // list remains valid
}
