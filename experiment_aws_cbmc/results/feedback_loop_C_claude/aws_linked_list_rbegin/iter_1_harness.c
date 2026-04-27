// === STEP 1: SUCCESS PATH ===
// aws_linked_list_rbegin always returns list->tail.prev (no failure path)
// The function simply returns the prev pointer of the tail node.
// Return value: list->tail.prev
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds and returns list->tail.prev
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_linked_list *)
//   - list->head.next: UNCHANGED always
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always (read but not modified)
// The function is read-only - it does not modify the list at all.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals list->tail.prev

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness(void) {
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
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* === STEP 1 assertions: return value is list->tail.prev === */
    assert(rval == old_tail_prev);

    /* === STEP 3 assertions: frame conditions - nothing changed === */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* === STEP 4 assertions: validity invariants === */
    assert(aws_linked_list_is_valid(&list));
}
