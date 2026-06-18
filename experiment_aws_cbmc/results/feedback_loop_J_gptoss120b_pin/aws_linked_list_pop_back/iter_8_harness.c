#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;

    bool was_empty = (list.head.next == &list.tail);

    aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (was_empty) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* After pop, the new tail.prev should be the previous node of the removed node */
        struct aws_linked_list_node *expected_new_tail_prev = old.tail.prev->prev;
        assert(list.tail.prev == expected_new_tail_prev);

        if (list.tail.prev != &list.head) {
            assert(list.tail.prev->next == &list.tail);
        } else {
            /* List became empty */
            assert(list.head.next == &list.tail);
        }
    }
}
