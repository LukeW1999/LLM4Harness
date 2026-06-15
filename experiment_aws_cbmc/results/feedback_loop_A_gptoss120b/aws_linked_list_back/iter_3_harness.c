#include <assert.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_back_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list old = list;

    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    assert(result == list.tail.prev);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
