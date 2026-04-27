// === STEP 1: SUCCESS PATH ===
// aws_linked_list_pop_front returns the front node (non-NULL pointer)
// The function removes the front element from the list:
//   - list->head.next: CHANGES to the node that was second in the list
//   - list->tail.prev: UNCHANGED (unless list had only one element, then points to head)
//   - The returned node's next: CHANGES to NULL (reset)
//   - The returned node's prev: CHANGES to NULL (reset)
//   - list remains valid after the call
//
// === STEP 2: FAILURE PATH ===
// No failure path - function has precondition that list is non-empty and valid
// The function always succeeds given valid preconditions
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED (points to what was second element, or tail if list had one element)
//   - head.prev: UNCHANGED (always NULL)
//   - tail.prev: CHANGED if list had one element (now points to head), else UNCHANGED
//   - tail.next: UNCHANGED (always NULL)
// returned node (struct aws_linked_list_node):
//   - next: CHANGED to NULL
//   - prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - returned node: next == NULL && prev == NULL (reset state)
//   - The list is valid and well-formed after the operation

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_pop_front_harness(void) {
    /* Allocate and initialize a linked list with at least one element */
    struct aws_linked_list list;
    /* Use ensure_linked_list_is_allocated to set up a valid list with some nodes */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions: list must be valid and non-empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state before the call */
    struct aws_linked_list_node *old_front = list.head.next;
    struct aws_linked_list_node *old_second = list.head.next->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_front(&list);

    /* Step 1 & 3: Verify the returned node is the old front */
    assert(result == old_front);

    /* The returned node must have been reset (next and prev are NULL) */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* Step 4: List must still be valid after the operation */
    assert(aws_linked_list_is_valid(&list));

    /* head.prev is always NULL */
    assert(list.head.prev == NULL);

    /* tail.next is always NULL */
    assert(list.tail.next == NULL);

    /* The new front of the list should be what was the second element */
    assert(list.head.next == old_second);

    /* The new front's prev should point back to head */
    assert(list.head.next->prev == &list.head);
}
