// === STEP 1: SUCCESS PATH ===
// aws_linked_list_pop_back returns the back node (non-NULL pointer to the removed node)
// The function:
//   1. Gets back = list->tail.prev (the last real node)
//   2. Calls aws_linked_list_remove(back) which:
//      - Sets back->prev->next = back->next
//      - Sets back->next->prev = back->prev
//      - Resets back: back->next = NULL, back->prev = NULL
//   3. Returns back
//
// After the call:
//   - list is still valid (aws_linked_list_is_valid)
//   - returned node has next == NULL and prev == NULL
//   - list may now be empty (if it had exactly one element)
//   - list->tail.prev points to what was previously back->prev
//
// === STEP 2: FAILURE PATH ===
// No failure path - function has precondition that list is non-empty and valid.
// The function always succeeds if preconditions hold.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED (if list had one element, now points to tail; otherwise unchanged or changed)
//   - head.prev: UNCHANGED (always NULL)
//   - tail.prev: CHANGED (now points to what was back->prev)
//   - tail.next: UNCHANGED (always NULL)
// returned node (back):
//   - next: CHANGED to NULL
//   - prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (postcondition of function)
//   - returned node: next == NULL && prev == NULL (postcondition)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness(void) {
    /* Allocate and initialize a non-empty linked list */
    struct aws_linked_list list;
    /* Use ensure_linked_list_is_allocated to set up a list with some nodes */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    /* Precondition: list must be non-empty */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = list.tail.prev->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* Postcondition: result is the node that was at the back */
    assert(result == old_back);

    /* Postcondition: returned node has next and prev reset to NULL */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* Postcondition: list is still valid */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: head.prev is still NULL */
    assert(list.head.prev == NULL);

    /* Postcondition: tail.next is still NULL */
    assert(list.tail.next == NULL);

    /* Postcondition: tail.prev now points to what was back->prev (old_back_prev) */
    assert(list.tail.prev == old_back_prev);

    /* Postcondition: the new last node's next points to tail */
    assert(list.tail.prev->next == &list.tail);
}
