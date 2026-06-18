#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = old.tail.prev;

    bool was_empty = aws_linked_list_empty(&list);

    struct aws_linked_list_node *out = NULL;
    aws_linked_list_pop_back(&list, &out);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list remains valid after pop_back");

    if (was_empty) {
        __CPROVER_assert(out == NULL, "out is NULL when list was empty");
        __CPROVER_assert(list.head.next == &list.tail && list.tail.prev == &list.head,
                         "list unchanged when empty");
    } else {
        __CPROVER_assert(out == old_last, "out points to the former last node");
        __CPROVER_assert(list.tail.prev == old_last->prev,
                         "tail.prev updated to previous node");
        __CPROVER_assert(list.tail.prev->next == &list.tail,
                         "new last node points to tail");
        __CPROVER_assert(out->next == NULL && out->prev == NULL,
                         "removed node is detached");
    }
}
