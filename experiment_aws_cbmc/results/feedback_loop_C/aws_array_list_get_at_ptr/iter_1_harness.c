// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_SUCCESS (or the successful value):
//   - *val: CHANGES to point to the element at index
//   - list: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_ERR (or fails):
//   - *val: UNCHANGED
//   - list: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - data: UNCHANGED always
//   val (void **):
//     - *val: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_headers.h>
#include <cbmc_proof/proof_allocators.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;
    void *val;
    size_t index;

    // Initialize list with some arbitrary values
    list.alloc = (struct aws_allocator *)can_fail_malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    // Initialize val with a nondeterministic value
    val = can_fail_malloc(MAX_ITEM_SIZE);

    // Initialize index with a nondeterministic value
    index = nondet_size_t();

    // Save old state of list
    struct aws_array_list old_list = list;
    void *old_val = val;

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(val != old_val); // *val should point to the element at index
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    } else {
        assert(val == old_val); // *val should remain unchanged
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
}
