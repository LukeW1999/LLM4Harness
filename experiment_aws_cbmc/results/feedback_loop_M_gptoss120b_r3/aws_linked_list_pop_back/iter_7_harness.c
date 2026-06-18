#include <assert.h>
#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_prev = NULL;
    if (old_last != &list.head) {
        old_prev = old_last->prev;
    }

    aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));

    if (old_last == &list.head) {
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(list.tail.prev == old_prev);
        assert(old_prev->next == &list.tail);
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
