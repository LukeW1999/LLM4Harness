// === STEP 1: SUCCESS PATH ===
// aws_linked_list_end always returns &list->tail (no failure path)
//   - Returns a pointer to list->tail
//   - The list itself is not modified
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list):
//   - head.next: UNCHANGED always
//   - head.prev: UNCHANGED always
//   - tail.next: UNCHANGED always
//   - tail.prev: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals &list->tail

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* Verify return value points to tail */
    assert(result == &list.tail);

    /* Verify frame conditions - list is unchanged */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Verify validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
