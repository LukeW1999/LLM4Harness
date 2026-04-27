#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_front_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* The list must be non-empty for front() to be meaningful.
     * aws_linked_list_front requires the list to be non-empty
     * (it returns head.next which should be a valid node, not tail). */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* 4. Assert postconditions */

    /* The function returns list->head.next */
    assert(result == old_head_next);
    assert(result == list.head.next);

    /* The result should not be NULL (since list is valid and non-empty) */
    assert(result != NULL);

    /* The result should not be the tail sentinel (list is non-empty) */
    assert(result != &list.tail);

    /* 5. Assert unchanged fields - function only reads, never writes */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
