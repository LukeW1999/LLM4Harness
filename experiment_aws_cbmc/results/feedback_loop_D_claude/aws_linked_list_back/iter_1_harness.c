// === STEP 1: SUCCESS PATH ===
// aws_linked_list_back returns list->tail.prev (the last node before tail)
// This is a simple getter - it returns a pointer, no state changes occur
// The function just reads list->tail.prev and returns it
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns list->tail.prev
// No state is modified
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (const struct aws_linked_list *)
//   - list->head.next: UNCHANGED always
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function is read-only, nothing changes
//
// === STEP 4: VALIDITY INVARIANTS ===
// - aws_linked_list_is_valid(list): YES (must hold before and after call)
// - Return value must equal list->tail.prev
// - The list must not be empty for back() to be meaningful (tail.prev != &head)

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_back_harness(void) {
    /* Allocate and initialize a linked list with nondeterministic content */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Precondition: list must not be empty (back() requires at least one element) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* === STEP 1 assertions: return value is correct === */
    assert(result == old_tail_prev);
    assert(result != NULL);

    /* === STEP 3 assertions: frame conditions - nothing changed === */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* === STEP 4 assertions: validity invariants === */
    assert(aws_linked_list_is_valid(&list));

    /* Additional: result should not be the head (list is non-empty) */
    assert(result != &list.head);
}
