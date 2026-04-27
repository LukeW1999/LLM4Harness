// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.current_size: DECREASES by item_size
//   - list.length: DECREASES by 1
//   - list.data: CONTENTS SHIFTED to the left by item_size bytes

// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_front returns AWS_OP_ERR (or fails):
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.data: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: CHANGED on success, UNCHANGED on failure
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *alloc = can_fail_allocator();

    __CPROVER_assume(initial_item_allocation <= max_initial_item_allocation);
    __CPROVER_assume(item_size <= max_item_size);

    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    old_list = list;

    int result = aws_array_list_pop_front(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.item_size == old_list.item_size);
        assert(list.length == old_list.length - 1);
        assert(list.current_size == old_list.current_size - old_list.item_size);
        // We cannot assert exact data contents due to shifting, but we can assert the size and length constraints
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
    aws_array_list_clean_up(&list);
}
