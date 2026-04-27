// === STEP 1: SUCCESS PATH ===
// aws_linked_list_front returns list->head.next (the first node in the list)
// This is a pure read operation - no state changes occur
// The return value is list->head.next
//
// === STEP 2: FAILURE PATH ===
// No failure path - this function always returns list->head.next
// (precondition: list must not be empty, but no error return)
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_linked_list *)
//   - list->head.next: UNCHANGED always (read only)
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function is a pure getter - nothing changes
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals list->head.next

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_front_harness(void) {
    /* Allocate and initialize a linked list with at least 1 element */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Precondition: list must not be empty (front requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* Postcondition: return value equals list->head.next */
    assert(result == old_head_next);

    /* Frame conditions: nothing changed */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Validity invariant: list is still valid after the call */
    assert(aws_linked_list_is_valid(&list));
}
