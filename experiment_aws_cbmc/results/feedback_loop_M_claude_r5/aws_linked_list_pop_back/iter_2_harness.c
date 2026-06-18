#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness() {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* pop_back requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = list.tail.prev->prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* 4. Assert postconditions */

    /* The returned node should be the old back node */
    assert(result == old_back);

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The new back should be the node that was before old_back */
    assert(list.tail.prev == old_back_prev);
    assert(old_back_prev->next == &list.tail);

    /* List invariants */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
