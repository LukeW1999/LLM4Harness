#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;
    bool was_empty = aws_linked_list_empty(&list);

    struct aws_linked_list_node *popped = NULL;
    int rv = aws_linked_list_pop_front(&list, &popped);

    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        assert(rv != 0);
        assert(popped == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(rv == 0);
        assert(popped == old_first);
        assert(list.head.next == old_first->next);
        assert(list.tail.prev == old_last);
        if (old_first == old_last) {
            assert(aws_linked_list_empty(&list));
        } else {
            assert(!aws_linked_list_empty(&list));
        }
    }
}
