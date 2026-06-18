#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_ITEMS 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_ITEMS);

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_front = list.head.next;

    aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(list.head.next != &list.tail);
        assert(list.head.next != old_front);
        assert(list.head.next->prev == &list.head);
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
