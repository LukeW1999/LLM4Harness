// === STEP 1: SUCCESS PATH ===
// aws_linked_list_rbegin always returns list->tail.prev (no failure path)
// The function simply reads list->tail.prev and returns it.
// No fields are modified - this is a pure read operation.
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always returns list->tail.prev.
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_linked_list *):
//   - head.next: UNCHANGED always
//   - head.prev: UNCHANGED always
//   - tail.next: UNCHANGED always
//   - tail.prev: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value equals list->tail.prev

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness(void) {
    /* Allocate and initialize a linked list with nondeterministic contents */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old = list;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* === STEP 1 assertions: return value is list->tail.prev === */
    assert(rval == old.tail.prev);

    /* === STEP 3 assertions: frame conditions - nothing changed === */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* === STEP 4 assertions: validity invariants === */
    assert(aws_linked_list_is_valid(&list));
}
