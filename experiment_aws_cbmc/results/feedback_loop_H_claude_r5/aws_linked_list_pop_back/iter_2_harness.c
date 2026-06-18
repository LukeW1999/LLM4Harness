#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness(void) {
    /* 1. Set up the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Only pop if list is non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = old_back->prev;

    /* 4. Call function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    /* 5. Assert postconditions */

    /* The returned node should be the old back */
    assert(popped == old_back);

    /* The new tail.prev should be the old back's prev */
    assert(list.tail.prev == old_back_prev);

    /* The new last element's next should point to tail */
    assert(old_back_prev->next == &list.tail);

    /* Head's prev should still be NULL */
    assert(list.head.prev == NULL);

    /* Tail's next should still be NULL */
    assert(list.tail.next == NULL);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
