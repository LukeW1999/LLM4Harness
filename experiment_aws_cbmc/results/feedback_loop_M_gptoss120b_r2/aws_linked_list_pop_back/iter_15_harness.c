#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_tail = list.tail;
    struct aws_linked_list_node *out = NULL;

    aws_linked_list_pop_back(&list, &out);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list valid after pop_back");

    if (aws_linked_list_empty(&old)) {
        __CPROVER_assert(out == NULL, "out is NULL when list was empty");
        __CPROVER_assert(aws_linked_list_empty(&list), "list remains empty");
    } else {
        __CPROVER_assert(out == old_tail, "out points to the former tail");
        __CPROVER_assert(list.head == old.head, "head unchanged after pop_back");

        if (old_tail->prev != NULL) {
            __CPROVER_assert(list.tail == old_tail->prev, "tail updated to previous node");
            __CPROVER_assert(list.tail->next == NULL, "new tail's next is NULL");
        } else {
            __CPROVER_assert(aws_linked_list_empty(&list), "list becomes empty after removing sole element");
            __CPROVER_assert(list.head == NULL && list.tail == NULL, "head and tail are NULL when list is empty");
        }
    }
}
