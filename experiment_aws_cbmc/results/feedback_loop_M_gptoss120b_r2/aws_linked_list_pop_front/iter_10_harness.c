#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        assert(popped == NULL);
        assert(list.head.next == old_first);
        assert(list.tail.prev == old_last);
    } else {
        assert(popped == old_first);
        struct aws_linked_list_node *new_first = list.head.next;
        assert(new_first == old_first->next);
        if (old_first == old_last) {
            assert(aws_linked_list_empty(&list));
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            assert(list.tail.prev == old_last);
            assert(new_first->prev == &list.head);
        }
    }
}
