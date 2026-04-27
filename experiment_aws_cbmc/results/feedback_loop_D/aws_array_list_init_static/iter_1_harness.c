// === STEP 1: SUCCESS PATH ===
// When aws_array_list_init_static returns AWS_OP_SUCCESS (or the successful value):
//   - list->alloc: CHANGES to NULL
//   - list->current_size: CHANGES to item_count * item_size
//   - list->length: CHANGES to 0
//   - list->item_size: CHANGES to item_size
//   - list->data: CHANGES to raw_array
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_init_static returns AWS_OP_ERR (or fails):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: CHANGED on success, UNCHANGED on failure
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list = list;
    void *raw_array = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(raw_array != NULL);
    size_t item_count = nondet_size_t();
    __CPROVER_assume(item_count > 0 && item_count <= MAX_BUFFER_SIZE);
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_BUFFER_SIZE);

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    // Assertions for success path
    assert(list.alloc == NULL);
    size_t expected_current_size;
    assert(!aws_mul_size_checked(item_count, item_size, &expected_current_size));
    assert(list.current_size == expected_current_size);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.data == raw_array);

    // Assertions for frame conditions
    assert(list.alloc == NULL || list.alloc == old_list.alloc);
    assert(list.current_size == expected_current_size || list.current_size == old_list.current_size);
    assert(list.length == 0 || list.length == old_list.length);
    assert(list.item_size == item_size || list.item_size == old_list.item_size);
    assert(list.data == raw_array || list.data == old_list.data);

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
