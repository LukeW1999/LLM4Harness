// === STEP 1: SUCCESS PATH ===
// aws_linked_list_rend always returns &list->head (no failure path)
// The function returns a pointer to the head node of the list.
// No fields are changed - this is a pure read operation.
//
// === STEP 2: FAILURE PATH ===
// No failure path exists for this function.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_linked_list *)
//   - list->head.next: UNCHANGED always
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function does not modify any fields.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - aws_linked_list_is_valid(list): YES (must hold before and after call)
// - Return value must equal &list->head

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rend_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state for frame condition checks */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* Call the function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* Step 1: Verify return value is &list->head */
    assert(rend == &list.head);

    /* Step 3: Frame conditions - no fields should be modified */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* Step 4: Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
