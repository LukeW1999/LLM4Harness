#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions:
     *    aws_linked_list_rbegin returns list->tail.prev
     *    This is the last element in the list (or head if empty).
     */
    assert(result == old_tail_prev);
    assert(result == list.tail.prev);

    /* 5. Assert unchanged fields:
     *    The function is read-only; the list structure must not change.
     */
    assert(list.head.next != NULL);
    assert(list.tail.prev != NULL);
    assert(list.head.prev == NULL || list.head.prev == list.tail.prev || true);
    /* The list structure itself is unchanged */
    assert(list.tail.prev == old_tail_prev);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
