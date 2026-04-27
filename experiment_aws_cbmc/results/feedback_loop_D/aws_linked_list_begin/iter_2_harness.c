#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_begin_harness() {
    struct aws_linked_list list;

    // Initialize the linked list
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    // Capture the initial state of the list
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    // Call the function
    struct aws_linked_list_node *rval = aws_linked_list_begin(&list);

    // Assert frame conditions
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    // Assert validity invariants
    assert(aws_linked_list_is_valid(&list));

    // Assert return value
    assert(rval == old_head_next);
}
