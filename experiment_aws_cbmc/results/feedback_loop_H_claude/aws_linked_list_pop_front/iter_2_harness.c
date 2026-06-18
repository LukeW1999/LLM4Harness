#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Assume the list is non-empty (pop_front requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_front_next = old_front->next;

    /* 4. Call function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* 5. Assert postconditions */

    /* The returned node should be the old front */
    assert(popped == old_front);

    /* The new front should be the old front's next */
    assert(list.head.next == old_front_next);

    /* The new front's prev should point to head */
    assert(old_front_next->prev == &list.head);

    /* head.prev is always NULL */
    assert(list.head.prev == NULL);

    /* tail.next is always NULL */
    assert(list.tail.next == NULL);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
