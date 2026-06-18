#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_after_harness(void) {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *after = &list.head;
    __CPROVER_assume(after != NULL);

    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;
    struct aws_linked_list_node *to_add_ptr = &to_add;

    struct aws_linked_list_node *old_after_prev = after->prev;
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    aws_linked_list_insert_after(after, to_add_ptr);

    assert(after->next == to_add_ptr);
    assert(to_add_ptr->prev == after);
    assert(to_add_ptr->next == old_after_next);
    assert(old_after_next->prev == to_add_ptr);
    assert(after->prev == old_after_prev);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(aws_linked_list_is_valid(&list));
}
