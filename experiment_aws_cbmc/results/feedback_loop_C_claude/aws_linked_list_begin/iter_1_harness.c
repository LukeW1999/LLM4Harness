// === STEP 1: SUCCESS PATH ===
// aws_linked_list_begin always returns list->head.next (no failure path)
// The function simply returns the next pointer of the head node.
// Return value: list->head.next (the first node in the list)
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds and returns list->head.next
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_linked_list *)
//   - list->head.next: UNCHANGED (only read, not written)
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function is read-only; no fields are modified.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals list->head.next

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness(void) {
    /* Allocate and initialize a linked list with nondeterministic length */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* STEP 1: Verify return value is list->head.next */
    assert(result == old_head_next);

    /* STEP 3: Verify frame conditions - no fields modified */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* STEP 4: Verify validity invariant holds after call */
    assert(aws_linked_list_is_valid(&list));
}
