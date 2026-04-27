// === STEP 1: SUCCESS PATH ===
// aws_linked_list_front returns list->head.next (the first element node pointer)
// This is a simple getter - it just returns list->head.next
// No state changes occur - this is a read-only operation
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns list->head.next
// The list is not modified in any way
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (const struct aws_linked_list *)
//   - list->head.next: UNCHANGED always (read-only)
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// Return value: equals list->head.next
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - The returned node pointer equals list->head.next
//   - The list must be non-empty for the result to be meaningful (not tail)
//   - list structure is unchanged after the call

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_front_harness(void) {
    /* Allocate and initialize a linked list with some elements */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Precondition: list must be non-empty (front requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state before the call */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_front(&list);

    /* === Assertions for Step 1: Return value correctness === */
    /* The returned value must equal list->head.next */
    assert(result == old_head_next);

    /* === Assertions for Step 3: Frame conditions === */
    /* All list fields must be unchanged (read-only operation) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* === Assertions for Step 4: Validity invariants === */
    /* List must still be valid after the call */
    assert(aws_linked_list_is_valid(&list));

    /* The result should not be NULL (list is non-empty, head.next is valid) */
    assert(result != NULL);

    /* The result should not be the tail sentinel (list is non-empty) */
    assert(result != &list.tail);
}
