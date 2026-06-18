#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));  /* ensure at least one element exists */

    /* 2. Pick a node from the list (first element) and save its old state */
    struct aws_linked_list_node *n = aws_linked_list_begin(&list);
    struct aws_linked_list_node *old_next = n->next;
    struct aws_linked_list_node *old_prev = n->prev;

    /* Save old list state for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    struct aws_linked_list_node *returned = aws_linked_list_next(n);

    /* 4. Assert postconditions */

    /* Return value matches node's next pointer */
    assert(returned == old_next);

    /* Node fields unchanged (const correctness) */
    assert(n->next == old_next);
    assert(n->prev == old_prev);

    /* List sentinels unchanged */
    assert(list.head.next == old_list.head.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* 5. Validity invariant still holds */
    assert(aws_linked_list_is_valid_deep(&list));
}
