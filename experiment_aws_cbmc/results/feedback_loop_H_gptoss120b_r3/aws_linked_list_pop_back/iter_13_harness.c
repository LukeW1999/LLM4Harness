#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_back = NULL;
    if (!aws_linked_list_empty(&list)) {
        old_back = list.tail.prev;
    }

    struct aws_linked_list_node *out = NULL;
    bool result = aws_linked_list_pop_back(&list, &out);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list remains valid");

    if (old_back == NULL) {
        __CPROVER_assert(!result, "pop_back should return false on empty list");
        __CPROVER_assert(out == NULL, "out should be NULL when list is empty");
        __CPROVER_assert(list.head.next == old.head.next, "head next unchanged on empty list");
        __CPROVER_assert(list.tail.prev == old.tail.prev, "tail prev unchanged on empty list");
    } else {
        __CPROVER_assert(result, "pop_back should return true on non‑empty list");
        __CPROVER_assert(out == old_back, "out should be the old back node");
        __CPROVER_assert(list.tail.prev == old_back->prev, "new tail prev is old back's prev");
        __CPROVER_assert(list.tail.prev->next == &list.tail, "new back next points to tail");
        __CPROVER_assert(list.head.prev == NULL, "head prev is NULL");
        __CPROVER_assert(list.tail.next == NULL, "tail next is NULL");
    }
}
