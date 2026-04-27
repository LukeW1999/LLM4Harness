#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
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

    int result = aws_array_list_init_static(&list, raw_array, item_count, item_size);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == NULL);
        size_t expected_current_size;
        assert(!aws_mul_size_checked(item_count, item_size, &expected_current_size));
        assert(list.current_size == expected_current_size);
        assert(list.length == 0);
        assert(list.item_size == item_size);
        assert(list.data == raw_array);
    }

    // Assertions for frame conditions
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == NULL);
    } else {
        assert(list.alloc == old_list.alloc);
    }
    if (result == AWS_OP_SUCCESS) {
        size_t expected_current_size;
        assert(!aws_mul_size_checked(item_count, item_size, &expected_current_size));
        assert(list.current_size == expected_current_size);
    } else {
        assert(list.current_size == old_list.current_size);
    }
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_list.length);
    }
    if (result == AWS_OP_SUCCESS) {
        assert(list.item_size == item_size);
    } else {
        assert(list.item_size == old_list.item_size);
    }
    if (result == AWS_OP_SUCCESS) {
        assert(list.data == raw_array);
    } else {
        assert(list.data == old_list.data);
    }

    // Validity invariant
    assert(aws_array_list_is_valid(&list));
}
