#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness() {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* pop_front requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* 4. Assert postconditions */

    /* The returned node should be the old front */
    assert(result == old_front);

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new front should be the old second node */
    assert(list.head.next == old_second);
    assert(old_second->prev == &list.head);
}
