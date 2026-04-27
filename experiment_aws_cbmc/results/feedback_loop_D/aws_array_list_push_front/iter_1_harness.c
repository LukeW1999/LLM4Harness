// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to orig_len + 1
//   - list.data: CHANGES to have val at the front and original data shifted
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_front returns AWS_OP_ERR (or fails):
//   - list.alloc: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   list (struct type):
//     - alloc: UNCHANGED always
//     - current_size: CHANGED on success if capacity increased, UNCHANGED otherwise
//     - length: CHANGED on success, UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - data: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    struct aws_array_list old_list;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    size_t max_initial_item_allocation = MAX_INITIAL_ITEM_ALLOCATION;
    size_t max_item_size = MAX_ITEM_SIZE;
    void *val = bounded_malloc(item_size);

    __CPROVER_assume(item_size > 0 && item_size <= max_item_size);
    __CPROVER_assume(initial_item_allocation <= max_initial_item_allocation);
    __CPROVER_assume(val != NULL);

    aws_array_list_init_dynamic(&list, NULL, initial_item_allocation, item_size);
    old_list = list;

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(memcmp(list.data, val, item_size) == 0);
        if (old_list.length > 0) {
            assert(memcmp((uint8_t *)list.data + item_size, old_list.data, old_list.length * item_size) == 0);
        }
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
}
