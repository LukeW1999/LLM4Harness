#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *out = NULL;

    bool result = aws_linked_list_pop_front(&list, &out);

    assert(aws_linked_list_is_valid(&list));

    bool was_empty = (old.head.next == &old.tail);

    if (was_empty) {
        assert(!result);
        assert(out == NULL);
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        assert(result);
        assert(out == old.head.next);
        assert(list.head.next == old.head.next->next);
        assert(list.head.next->prev == &list.head);
        assert(list.tail.prev == old.tail.prev);
    }
}
