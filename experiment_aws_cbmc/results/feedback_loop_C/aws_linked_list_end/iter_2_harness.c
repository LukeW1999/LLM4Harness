#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;

    // Ensure the list is allocated and valid
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    old_list = list;

    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    // Assert frame conditions
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    // Assert validity invariants
    assert(aws_linked_list_is_valid(&list));
}
