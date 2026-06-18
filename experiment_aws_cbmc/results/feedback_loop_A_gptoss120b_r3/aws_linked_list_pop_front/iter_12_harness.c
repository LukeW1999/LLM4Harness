#include <assert.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    struct aws_linked_list_node *out = NULL;
    bool result = aws_linked_list_pop_front(&list, &out);

    assert(aws_linked_list_is_valid(&list));

    if (result) {
        assert(out == old_head_next);
        assert(list.head.next == old_head_next->next);
        if (list.head.next != &list.tail) {
            assert(list.head.next->prev == &list.head);
        } else {
            assert(list.tail.prev == &list.head);
        }
    } else {
        assert(out == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    }
}
