#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    /* 1. Declare and initialize a linked list */
    struct aws_linked_list list;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* pop_back requires the list to be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 2. Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *new_back = old_back->prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* 4. Assert postconditions */

    /* Validity invariant must hold after the call */
    assert(aws_linked_list_is_valid(&list));

    /* The returned node should be the old back */
    assert(result == old_back);

    /* The new back of the list should be what was before old_back */
    assert(list.tail.prev == new_back);

    /* new_back's next should now point to tail */
    assert(new_back->next == &list.tail);
}
