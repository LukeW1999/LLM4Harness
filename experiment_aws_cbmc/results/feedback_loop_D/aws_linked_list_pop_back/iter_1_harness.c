// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_pop_back returns AWS_OP_SUCCESS (or the successful value):
//   - list.head.next: CHANGES to the next node of the popped node
//   - list.tail.prev: CHANGES to the previous node of the popped node if list was not empty, otherwise remains &list.head
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_pop_back returns AWS_OP_ERR (or fails):
//   - list.head.next: UNCHANGED
//   - list.tail.prev: UNCHANGED
//   - list.head.prev: UNCHANGED (always NULL)
//   - list.tail.next: UNCHANGED (always NULL)
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_linked_list):
//     - head.next: CHANGED on success, UNCHANGED on failure
//     - head.prev: UNCHANGED always
//     - tail.prev: CHANGED on success, UNCHANGED on failure
//     - tail.next: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_is_valid(&list): YES (must hold after call)
```

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    struct aws_linked_list old_list = list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(!aws_linked_list_empty(&list)); // Ensure list is not empty for successful pop

    struct aws_linked_list_node *popped_node = aws_linked_list_pop_back(&list);

    // Success path assertions
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next != old_list.head.next);
    assert(list.tail.prev != old_list.tail.prev || aws_linked_list_empty(&list));
    assert(popped_node->next == NULL);
    assert(popped_node->prev == NULL);

    // Failure path assertions (not applicable here as we assume list is not empty)
    // If list were empty, aws_linked_list_pop_back would not be called, so these are implicit

    // Frame conditions
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    // Validity invariants
    assert(aws_linked_list_is_valid(&list));
}
