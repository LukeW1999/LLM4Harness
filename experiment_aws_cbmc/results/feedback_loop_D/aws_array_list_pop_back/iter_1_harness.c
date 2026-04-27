// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_back returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to list.length - 1
//   - list.data[last_item_offset]: CHANGES to 0 (memset to 0)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_back returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.data[last_item_offset]: UNCHANGED
//   - list.data: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct aws_array_list):
//     - length: CHANGED on success, UNCHANGED on failure
//     - data: CHANGED on success (last item is memset to 0), UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - current_size: UNCHANGED always
//     - alloc: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length - 1);
        size_t last_item_offset = old_list.item_size * (old_list.length - 1);
        assert(((uint8_t *)list.data)[last_item_offset] == 0);
    } else {
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.current_size == old_list.current_size);
        assert(list.alloc == old_list.alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
