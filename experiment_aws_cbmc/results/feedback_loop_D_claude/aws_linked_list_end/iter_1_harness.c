// === STEP 1: SUCCESS PATH ===
// aws_linked_list_end always returns &list->tail (a pointer to the tail node)
// There is no failure path - the function always succeeds.
// The function returns a const pointer to list->tail.
// No fields of list are modified.
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always returns &list->tail.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (const struct aws_linked_list *)
//   - list->head.next: UNCHANGED always
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function is read-only; nothing changes.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value must equal &list->tail

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old_list = list;

    /* Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* STEP 1: Verify return value is &list->tail */
    assert(result == &list.tail);

    /* STEP 3: Frame conditions - nothing should have changed */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.tail.prev == old_list.tail.prev);

    /* STEP 4: Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
