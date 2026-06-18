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

    /* aws_linked_list_back requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 4. Assert postconditions */

    /* The function returns list->tail.prev */
    assert(result == old_tail_prev);

    /* The result should not be NULL (list is non-empty, so tail.prev != &head) */
    assert(result != NULL);

    /* The result should not be the head sentinel */
    assert(result != &list.head);

    /* 5. Assert unchanged fields - the list structure itself must not change */
    assert(list.tail.prev == old_tail_prev);
    assert(list.head.next == old_head_next);
    assert(list.tail.next == old_tail_next);
    assert(list.head.prev == old_head_prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));

    /* The list should still be non-empty */
    assert(!aws_linked_list_empty(&list));
}
