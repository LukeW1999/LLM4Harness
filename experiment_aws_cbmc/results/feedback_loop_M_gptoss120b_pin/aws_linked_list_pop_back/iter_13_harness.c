/* Bounding constant defined in the Makefile */
#ifndef MAX_LINKED_LIST_ITEM_ALLOCATION
#define MAX_LINKED_LIST_ITEM_ALLOCATION 5
#endif

#include <aws/common/linked_list.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_tail = list.tail;
    bool was_empty = (list.head == NULL);

    /* Invoke the function under test */
    aws_linked_list_pop_back(&list);

    /* Verify list invariants */
    assert(aws_linked_list_is_valid(&list));

    if (was_empty) {
        /* List should remain empty */
        assert(list.head == NULL);
        assert(list.tail == NULL);
    } else {
        /* Tail should have moved back */
        struct aws_linked_list_node *new_tail = list.tail;
        assert(new_tail == old_tail->prev);
        if (new_tail != NULL) {
            assert(new_tail->next == NULL);
        } else {
            /* List became empty */
            assert(list.head == NULL);
        }
    }
}
