// === STEP 1: SUCCESS PATH ===
// When aws_array_list_clean_up returns AWS_OP_SUCCESS (or the successful value):
//   - list->alloc: UNCHANGED
//   - list->current_size: SET to 0
//   - list->length: SET to 0
//   - list->item_size: SET to 0
//   - list->data: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_clean_up returns AWS_OP_ERR (or fails):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
```

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;

    // Initialize list with non-deterministic values
    list.alloc = (struct aws_allocator *) nondet_ptr();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = (void *) nondet_ptr();

    // Ensure the list is bounded for safety
    ensure_array_list_has_allocated_data_member(&list);

    aws_array_list_clean_up(&list);

    // Assertions based on the analysis
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    // Check if the list is valid after cleanup
    assert(aws_array_list_is_valid(&list));
}
