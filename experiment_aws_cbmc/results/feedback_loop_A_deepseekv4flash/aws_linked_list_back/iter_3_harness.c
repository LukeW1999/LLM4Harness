#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    // Initialize list as empty
    aws_linked_list_init(&list);
    // Add one node to make list non-empty
    aws_linked_list_push_back(&list, &node);

    // Ensure list is valid and non-empty
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    // Save old state
    struct aws_linked_list old_list = list;

    // Call function
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    // Postconditions
    assert(result == list.tail.prev);
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
