#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Add a node to the list to ensure it's not empty */
    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);
    aws_linked_list_push_back(&list, &node);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *popped_node = aws_linked_list_pop_front(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: node is removed from the front of the list */
    assert(popped_node == &node); // Correct node is popped
    if (old_list.head.next == &node) {
        assert(list.head.next == &list.tail); // Head points to tail if list had only one node
    } else {
        assert(list.head.next == node.next); // Head points to next node
        assert(node.next->prev == &list.head); // Next node points back to head
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.tail.prev == old_list.tail.prev); // tail.prev remains unchanged
    assert(list.tail.next == old_list.tail.next); // tail.next remains unchanged

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    if (popped_node != NULL) {
        assert(!aws_linked_list_node_is_in_list(popped_node));
    }
}
