// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: CHANGES to the value at the specified index in list

// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - length: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: UNCHANGED always
//   - alloc: UNCHANGED always
// val (void*):
//   - CHANGES on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_get_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    size_t index;
    uint8_t val[list.item_size];
    __CPROVER_assume(index < list.length); // Ensure index is within bounds for success case

    struct aws_array_list old_list = list;
    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should now contain the value at the specified index
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should remain unchanged
    }

    assert(aws_array_list_is_valid(&list));
}
