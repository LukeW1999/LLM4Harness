// === STEP 1: SUCCESS PATH ===
// aws_linked_list_back returns list->tail.prev (the last node in the list)
// This is a pure read operation - no state changes occur
// The return value is list->tail.prev
//
// === STEP 2: FAILURE PATH ===
// No failure path - this function always returns list->tail.prev
// (precondition: list must not be empty, but no error code returned)
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: UNCHANGED always
//   - head.prev: UNCHANGED always
//   - tail.next: UNCHANGED always
//   - tail.prev: UNCHANGED always
// The function is a pure read - nothing is modified
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals list->tail.prev
//   - List must be non-empty (back() requires non-empty list)

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness(void) {
    /* Allocate and initialize a linked list with at least one element */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Precondition: list must not be empty (back() requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* === Postconditions === */

    /* Return value must equal list->tail.prev */
    assert(result == list.tail.prev);

    /* Return value must not be NULL (list is non-empty) */
    assert(result != NULL);

    /* Frame conditions: nothing in the list was modified */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* List validity must still hold */
    assert(aws_linked_list_is_valid(&list));

    /* The result should not be the tail sentinel itself (list is non-empty) */
    assert(result != &list.tail);
}
