#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list;

    // Initialize the list and make a copy before modification
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    old_list = list;

    // Call the function under test
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    // Assert frame conditions and validity invariants
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    if (result != NULL) {
        assert(list.head.next == result);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    assert(aws_linked_list_is_valid(&list));
}
