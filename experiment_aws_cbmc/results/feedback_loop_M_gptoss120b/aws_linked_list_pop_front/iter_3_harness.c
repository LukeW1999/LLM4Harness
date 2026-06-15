#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    struct aws_linked_list list;

    /* Allocate and bound the linked list */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy for immutability checks */
    struct aws_linked_list old = list;

    /* Call the function under test */
    aws_linked_list_pop_front(&list);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&list));

    if (aws_linked_list_empty(&old)) {
        /* List was empty, should be unchanged */
        assert(list.head.next == old.head.next);
        assert(list.tail.prev == old.tail.prev);
    } else {
        /* List was non‑empty, first element removed */
        struct aws_linked_list_node *first = old.head.next;
        assert(list.head.next == first->next);
        /* Tail's prev should stay the same unless the list became empty */
        if (aws_linked_list_empty(&list)) {
            assert(list.head.next == &list.tail);
            assert(list.tail.prev == &list.head);
        } else {
            assert(list.tail.prev == old.tail.prev);
        }
    }
}
