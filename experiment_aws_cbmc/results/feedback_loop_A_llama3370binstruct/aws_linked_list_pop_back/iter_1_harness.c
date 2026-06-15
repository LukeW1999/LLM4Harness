#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    aws_linked_list_init(&list);

    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);

    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.prev == old.tail.prev);
    assert(list.tail.next == old.tail.next);

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
}
