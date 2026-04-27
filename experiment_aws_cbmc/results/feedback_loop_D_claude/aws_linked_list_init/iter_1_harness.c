// === STEP 1: SUCCESS PATH ===
// aws_linked_list_init always succeeds (void return, no failure path)
//   - list->head.next: CHANGES to &list->tail
//   - list->head.prev: CHANGES to NULL
//   - list->tail.prev: CHANGES to &list->head
//   - list->tail.next: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void and always succeeds.
//
// === STEP 3: FRAME CONDITIONS ===
// param: list (struct aws_linked_list *)
//   - list->head.next: CHANGED to &list->tail
//   - list->head.prev: CHANGED to NULL
//   - list->tail.prev: CHANGED to &list->head
//   - list->tail.next: CHANGED to NULL
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(list): YES (must hold after call)
//   - aws_linked_list_empty(list): YES (list must be empty after init)

#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_init_harness(void) {
    /* Allocate a linked list (no need for ensure_linked_list_is_allocated
       since we're testing init itself - just need a valid pointer) */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Call the function under test */
    aws_linked_list_init(list);

    /* === STEP 1 assertions: verify post-conditions === */
    /* head.next must point to tail */
    assert(list->head.next == &list->tail);
    /* head.prev must be NULL */
    assert(list->head.prev == NULL);
    /* tail.prev must point to head */
    assert(list->tail.prev == &list->head);
    /* tail.next must be NULL */
    assert(list->tail.next == NULL);

    /* === STEP 4 assertions: validity invariants === */
    /* List must be valid after init */
    assert(aws_linked_list_is_valid(list));
    /* List must be empty after init */
    assert(aws_linked_list_empty(list));
}
