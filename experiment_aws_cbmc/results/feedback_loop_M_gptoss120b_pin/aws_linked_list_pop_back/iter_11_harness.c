/* Bounding constant defined in the Makefile */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_tail = list.tail;
    bool was_empty = (list.head == NULL);

    struct aws_linked_list_node *removed = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        assert(removed == NULL);
        assert(list.head == NULL);
        assert(list.tail == NULL);
    } else {
        assert(removed == old_tail);
        struct aws_linked_list_node *new_tail = list.tail;
        assert(new_tail == old_tail->prev);
        if (new_tail != NULL) {
            assert(new_tail->next == NULL);
        } else {
            assert(list.head == NULL);
        }
    }
}
