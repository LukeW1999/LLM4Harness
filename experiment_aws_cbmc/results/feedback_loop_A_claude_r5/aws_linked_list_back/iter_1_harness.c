#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Precondition: list must not be empty (back() requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 5. Assert postconditions */

    /* The function returns the element at the back of the list,
       which is list->tail.prev */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* The result must not be NULL (list is non-empty) */
    assert(result != NULL);

    /* The result should not be the tail sentinel itself */
    assert(result != &list.tail);

    /* The list structure must not have changed */
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.next == old_head_next);

    /* The list must still be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* The list must still be non-empty */
    assert(!aws_linked_list_empty(&list));
}
