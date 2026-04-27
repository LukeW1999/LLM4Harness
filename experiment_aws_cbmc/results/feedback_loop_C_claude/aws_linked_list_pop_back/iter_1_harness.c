// === STEP 1: SUCCESS PATH ===
// aws_linked_list_pop_back returns the back node (non-NULL pointer)
// The list has the back node removed:
//   - list->tail.prev: CHANGES to the node before the removed node
//   - list->head.next: may CHANGE if list had only one element
//   - The returned node's next: CHANGES to NULL
//   - The returned node's prev: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function has precondition that list is non-empty
// (precondition: !aws_linked_list_empty(list) && aws_linked_list_is_valid(list))
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: CHANGED (if list had one element, now points to tail)
//   - head.prev: UNCHANGED (always NULL)
//   - tail.prev: CHANGED (points to new last element)
//   - tail.next: UNCHANGED (always NULL)
// returned node (struct aws_linked_list_node):
//   - next: CHANGED to NULL
//   - prev: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - returned node->next == NULL: YES
//   - returned node->prev == NULL: YES
//   - list is still valid: YES

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness(void) {
    /* Allocate and initialize a linked list with up to MAX_LINKED_LIST_ITEM_ALLOCATION nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_back = list.tail.prev;
    struct aws_linked_list_node *old_back_prev = list.tail.prev->prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_pop_back(&list);

    /* === Postconditions === */

    /* The returned node should be the old back node */
    assert(result == old_back);

    /* The returned node's next and prev should be NULL */
    assert(result->next == NULL);
    assert(result->prev == NULL);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The list's tail.prev should now point to the old back's prev */
    assert(list.tail.prev == old_back_prev);

    /* The list's tail.next should still be NULL */
    assert(list.tail.next == NULL);

    /* The list's head.prev should still be NULL */
    assert(list.head.prev == NULL);

    /* The new tail.prev->next should point to tail (bidirectional link) */
    assert(list.tail.prev->next == &list.tail);
}
