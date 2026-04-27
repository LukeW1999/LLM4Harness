// === STEP 1: SUCCESS PATH ===
// aws_linked_list_rend returns a pointer to list->head (the sentinel head node)
// This is always successful - it simply returns &list->head
// Return value: pointer equal to &list->head
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns &list->head
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (const struct aws_linked_list *)
//   - list->head.next: UNCHANGED always (function only reads)
//   - list->head.prev: UNCHANGED always
//   - list->tail.next: UNCHANGED always
//   - list->tail.prev: UNCHANGED always
// The function does not modify any fields - it only reads &list->head
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold before and after call)
//   - Return value must equal &list->head

#include <aws/common/linked_list.h>
#include <stddef.h>
#include <assert.h>

void aws_linked_list_rend_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* Call the function under test */
    const struct aws_linked_list_node *rend = aws_linked_list_rend(&list);

    /* Step 1: Verify return value is &list->head */
    assert(rend == &list.head);

    /* Step 3: Frame conditions - nothing should be modified */
    assert(list.head.next == old_head.next);
    assert(list.head.prev == old_head.prev);
    assert(list.tail.next == old_tail.next);
    assert(list.tail.prev == old_tail.prev);

    /* Step 4: Validity invariants */
    assert(aws_linked_list_is_valid(&list));
}
