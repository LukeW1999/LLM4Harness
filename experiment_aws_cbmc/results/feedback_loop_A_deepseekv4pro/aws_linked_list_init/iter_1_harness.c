#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

// Bound the maximum number of nodes that can be allocated in the list
#define MAX_LINKED_LIST_ITEM_ALLOCATION 2

void aws_linked_list_push_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    // Set up a valid list with a bounded number of possible nodes
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // The node to append must be a fresh, detached node
    node.prev = NULL;
    node.next = NULL;

    // Save the state that will be affected by the operation
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    // Call the function under verification
    aws_linked_list_push_back(&list, &node);

    // Postconditions translated from the Doxygen description and implementation

    // 1. The list remains valid after insertion
    assert(aws_linked_list_is_valid(&list));

    // 2. The list is never empty after a successful push_back
    assert(!aws_linked_list_empty(&list));

    // 3. The appended node is the new last element (tail->prev == node)
    assert(list.tail.prev == &node);

    // 4. The node's next pointer correctly links to the tail sentinel
    assert(node.next == &list.tail);

    // 5. The node's previous pointer is the former last element
    //    (which is the head if the list was originally empty)
    assert(node.prev == old_tail_prev);

    // 6. If the list was originally non-empty, head->next remains unchanged.
    //    If the list was empty, head->next now points to the new node.
    if (old_head_next == &list.tail) {
        // The list was empty, so the new node is also the first element
        assert(list.head.next == &node);
    } else {
        // The list had at least one element, head still points to the old first node
        assert(list.head.next == old_head_next);
    }

    // 7. Sentinel invariants that never change
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
