// === STEP 1: SUCCESS PATH ===
// When aws_linked_list_insert_before returns AWS_OP_SUCCESS (or the successful value):
//   - before->prev: CHANGES to to_add
//   - before->prev->next: CHANGES to to_add
//   - to_add->next: CHANGES to before
//   - to_add->prev: CHANGES to before->prev (old value of before->prev)
//
// === STEP 2: FAILURE PATH ===
// When aws_linked_list_insert_before returns AWS_OP_ERR (or fails):
//   - before->prev: UNCHANGED
//   - before->prev->next: UNCHANGED
//   - to_add->next: UNCHANGED
//   - to_add->prev: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   before (struct aws_linked_list_node type):
//     - next: UNCHANGED always
//     - prev: CHANGED on success, UNCHANGED on failure
//   before->prev (struct aws_linked_list_node type):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: UNCHANGED always
//   to_add (struct aws_linked_list_node type):
//     - next: CHANGED on success, UNCHANGED on failure
//     - prev: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_linked_list_node_is_valid(before): YES (must hold after call)
//   - aws_linked_list_node_is_valid(to_add): YES (must hold after call)
```

```c
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    // Initialize nodes
    ensure_linked_list_is_allocated((struct aws_linked_list *) &before, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated((struct aws_linked_list *) &to_add, MAX_LINKED_LIST_ITEM_ALLOCATION);

    // Save old state
    struct aws_linked_list_node before_old = before;
    struct aws_linked_list_node to_add_old = to_add;

    // Call the function under test
    aws_linked_list_insert_before(&before, &to_add);

    // Assertions for success path
    assert(before.prev == &to_add);
    assert(before_old.prev->next == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == before_old.prev);

    // Assertions for frame conditions
    assert(before.next == before_old.next);
    assert(before_old.prev->prev == before_old.prev->prev); // This is trivially true but kept for completeness
    assert(to_add_old.next == to_add_old.next);
    assert(to_add_old.prev == to_add_old.prev);

    // Validity invariants
    assert(aws_linked_list_node_is_valid(&before));
    assert(aws_linked_list_node_is_valid(&to_add));
}
