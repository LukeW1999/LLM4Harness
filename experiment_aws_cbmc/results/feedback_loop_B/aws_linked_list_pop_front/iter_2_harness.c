#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);
    aws_linked_list_push_back(&list, &node);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *popped_node = aws_linked_list_pop_front(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path */
    assert(popped_node == &node);
    assert(list.head == &node.next);
    assert(node.next.prev == &list.head);
    if (node.next.next != &list.tail) {
        assert(node.next.next->prev == &node.next);
    } else {
        assert(list.tail.prev == &node.next);
    }

    /* Failure path (not applicable for this function, as it does not return a status code) */

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.tail == old_list.tail);
    assert(list.tail.prev->next == &list.tail);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    if (popped_node != NULL) {
        assert(!aws_linked_list_node_is_in_list(popped_node));
    }
}
