#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_last = list.tail.prev;
    int was_empty = (list.head.next == &list.tail);

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    if (was_empty) {
        assert(popped == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(popped == old_last);
        assert(popped->next == NULL);
        assert(popped->prev == NULL);
        assert(list.tail.prev == old_last->prev);
        assert(list.tail.prev->next == &list.tail);
    }
}
