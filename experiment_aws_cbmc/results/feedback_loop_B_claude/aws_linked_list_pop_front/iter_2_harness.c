#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_linked_list_pop_front_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. The list must be non-empty to call pop_front */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = old_front->next;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* 5. Assert postconditions */

    /* The returned node should be the old front */
    assert(result == old_front);

    /* The new front should be what was the second element */
    assert(list.head.next == old_second);

    /* The new front's prev should point to head */
    assert(old_second->prev == &list.head);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* head.prev should still be NULL */
    assert(list.head.prev == NULL);

    /* tail.next should still be NULL */
    assert(list.tail.next == NULL);
}
