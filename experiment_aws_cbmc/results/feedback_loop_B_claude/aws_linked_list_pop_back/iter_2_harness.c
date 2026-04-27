#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_linked_list_pop_back_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. The list must be non-empty to call pop_back */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = old_back->prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* 5. Assert postconditions */

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The returned node must be the old last element */
    assert(result == old_back);

    /* The new tail.prev must be the old back's prev */
    assert(list.tail.prev == old_back_prev);

    /* The new last node's next must point to tail */
    assert(old_back_prev->next == &list.tail);

    /* head.prev must still be NULL */
    assert(list.head.prev == NULL);

    /* tail.next must still be NULL */
    assert(list.tail.next == NULL);
}
