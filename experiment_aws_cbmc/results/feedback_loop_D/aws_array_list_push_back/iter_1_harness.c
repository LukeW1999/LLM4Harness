// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_back returns AWS_OP_SUCCESS (or the successful value):
//   - list.data: CHANGES to point to new memory if reallocation occurs
//   - list.current_size: CHANGES if reallocation occurs
//   - list.length: CHANGES to length + 1
//   - list.data[length]: CHANGES to contain the value of val

// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_back returns AWS_OP_ERR (or fails):
//   - list.data: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.length: UNCHANGED
//   - list.data[length]: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - data: CHANGED on success if reallocation occurs, UNCHANGED on failure
//   - current_size: CHANGED on success if reallocation occurs, UNCHANGED on failure
//   - item_size: UNCHANGED always
//   - length: CHANGED on success, UNCHANGED on failure
//   - alloc: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(alloc != NULL);
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    struct aws_array_list old_list = list;
    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        if (old_list.current_size < list.length * list.item_size) {
            assert(list.data != old_list.data);
            assert(list.current_size >= list.length * list.item_size);
        } else {
            assert(list.data == old_list.data);
            assert(list.current_size == old_list.current_size);
        }
    } else {
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
    }

    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(aws_array_list_is_valid(&list));

    free(val);
    free(alloc);
}
