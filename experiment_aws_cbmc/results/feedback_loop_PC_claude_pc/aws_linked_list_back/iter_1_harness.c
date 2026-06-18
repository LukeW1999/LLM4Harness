#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* aws_linked_list_back requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Assert postconditions */

    /* RETURN: result must be list.tail.prev (the last element) */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* RETURN: result must not be NULL (list is non-empty) */
    assert(result != NULL);

    /* RETURN: result must not be the tail sentinel itself */
    assert(result != &list.tail);

    /* FRAME: the list structure must not have changed */
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.next == old_head_next);
    assert(list.tail.next == old_tail_next);
    assert(list.head.prev == old_head_prev);

    /* INVARIANTS: list must still be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* INVARIANTS: list must still be non-empty */
    assert(!aws_linked_list_empty(&list));

    /* INVARIANTS: result's next pointer must point to tail (it's the last element) */
    assert(result->next == &list.tail);

    /* INVARIANTS: tail's prev must still point to result */
    assert(list.tail.prev == result);
}
