#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_last = list.tail.prev;
    int was_empty = (list.head.next == &list.tail);

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));

    assert(list.head.prev == 0);
    assert(list.tail.next == 0);

    if (was_empty) {
        assert(popped == 0);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(popped == old_last);
        assert(popped->next == 0);
        assert(popped->prev == 0);
        assert(list.tail.prev == old_last->prev);
        assert(list.tail.prev->next == &list.tail);
    }
}
