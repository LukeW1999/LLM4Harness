// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: CHANGES to list.length + 1
//   - list.data: CHANGES to have the new element at the front
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
//     - current_size: CHANGED on success, UNCHANGED on failure
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
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *alloc = (struct aws_allocator *)nondet_voidp();
    __CPROVER_assume(item_size > 0 && initial_item_allocation > 0);
    __CPROVER_assume(aws_array_list_is_bounded(&list, initial_item_allocation, item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t *old_data = (uint8_t *)list.data;

    uint8_t val[item_size];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(memcmp(old_data + list.item_size, old_data, old_list.length * list.item_size) == 0);
        assert(memcmp(list.data, val, list.item_size) == 0);
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_data);
    }

    assert(aws_array_list_is_valid(&list));
}
