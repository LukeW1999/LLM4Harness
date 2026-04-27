// === STEP 1: SUCCESS PATH ===
// aws_linked_list_pop_front returns the front node (list->head.next before removal)
// After the call:
//   - list->head.next: CHANGES to the node that was second in the list (old front->next)
//   - list->tail.prev: UNCHANGED if list had more than one element; CHANGES to &list->head if list becomes empty
//   - The returned node's next: CHANGES to NULL (reset)
//   - The returned node's prev: CHANGES to NULL (reset)
//   - list remains valid
//
// === STEP 2: FAILURE PATH ===
// No failure path - function has precondition that list is non-empty and valid.
// If preconditions are met, it always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED (points to what was second element, or &list->tail if now empty)
//   - head.prev: UNCHANGED (always NULL)
//   - tail.prev: CHANGED (points to &list->head if list becomes empty, or unchanged if still has elements)
//   - tail.next: UNCHANGED (always NULL)
// returned node (struct aws_linked_list_node):
//   - next: CHANGED to NULL
//   - prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
//   - returned node->next == NULL: YES
//   - returned node->prev == NULL: YES

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness(void) {
    /* Allocate and initialize a linked list with at least 1 element */
    struct aws_linked_list list;
    /* Use ensure_linked_list_is_allocated with max_length >= 1 to ensure non-empty */
    ensure_linked_list_is_allocated(&list, 4);

    /* Preconditions: list must be valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* === Assertions from Step 1 === */
    /* The returned node is the old front */
    assert(result == old_front);

    /* The returned node's pointers are reset to NULL */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* list->head.next now points to what was the second element */
    assert(list.head.next == old_second);

    /* head.prev is always NULL */
    assert(list.head.prev == NULL);

    /* tail.next is always NULL */
    assert(list.tail.next == NULL);

    /* === Assertions from Step 4 === */
    /* List remains valid after the operation */
    assert(aws_linked_list_is_valid(&list));
}
