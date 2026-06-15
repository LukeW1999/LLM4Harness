#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old_list = list;

    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    if (result!= NULL) {
        assert(result == list.head.next);
    } else {
        assert(aws_linked_list_empty(&list));
    }

    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
