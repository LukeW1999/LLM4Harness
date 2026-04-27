// === STEP 1: SUCCESS PATH ===
// aws_linked_list_begin always returns list->head.next (no failure path)
// The function simply reads list->head.next and returns it.
// No fields are modified - this is a pure read operation.
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always returns list->head.next.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (const struct aws_linked_list *)
//   - list->head.next: UNCHANGED always (only read)
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
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
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* === STEP 1 assertions: return value is list->head.next === */
    assert(result == list.head.next);
    assert(result == old_list.head.next);

    /* === STEP 3 assertions: frame conditions - nothing changed === */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* === STEP 4 assertions: validity invariants === */
    assert(aws_linked_list_is_valid(&list));
}
