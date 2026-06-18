#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness(void) {
    /* 1. Set up the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. The list must be non-empty to call pop_front */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before the call */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* 4. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* 5. Assert postconditions */

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The returned node must be the old front */
    assert(result == old_front);

    /* The new front must be the old second node */
    assert(list.head.next == old_second);

    /* The new front's prev must point to head */
    assert(old_second->prev == &list.head);

    /* head.prev must still be NULL (invariant) */
    assert(list.head.prev == NULL);

    /* tail.next must still be NULL (invariant) */
    assert(list.tail.next == NULL);
}
