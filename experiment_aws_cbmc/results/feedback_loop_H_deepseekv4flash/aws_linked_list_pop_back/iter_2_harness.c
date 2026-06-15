#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    /* Non-deterministic list with at least one node */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list is valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    bool old_list_has_one_node = (old_head_next == old_tail_prev);

    /* Call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* Postconditions */
    assert(popped != NULL);
    assert(popped == old_tail_prev);
    assert(aws_linked_list_is_valid(&list));

    /* Check list structure after pop */
    if (old_list_has_one_node) {
        /* List becomes empty */
        assert(aws_linked_list_empty(&list));
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List still has at least one node */
        assert(!aws_linked_list_empty(&list));
        /* The new tail.prev is the node before the popped node */
        assert(list.tail.prev == old_tail_prev->prev);
        /* The head remains unchanged */
        assert(list.head.next == old_head_next);
    }

    /* Sentinels remain valid */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
