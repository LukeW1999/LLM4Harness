#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_pop_front_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Only call pop_front if list is non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before calling */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* 4. Call function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* 5. Assert postconditions */

    /* The returned node is the old front */
    assert(popped == old_front);

    /* The new front is what was the second element */
    assert(list.head.next == old_second);

    /* The new front's prev points to head */
    assert(old_second->prev == &list.head);

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));
}
