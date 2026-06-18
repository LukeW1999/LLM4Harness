#include <assert.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;
    struct aws_linked_list_node *old_prev = old_last->prev;

    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    assert(removed == old_last);
    assert(removed->next == NULL);
    assert(removed->prev == NULL);

    if (old_prev == &list.head) {
        assert(list.tail.prev == &list.head);
        assert(list.head.next == &list.tail);
    } else {
        assert(list.tail.prev == old_prev);
    }

    assert(list.head.next == old.head.next);
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_node_is_in_list(removed));
}
