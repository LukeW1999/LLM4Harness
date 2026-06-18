#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t expected_allocation_size = 0;
    int multiplication_result = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation_size);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (multiplication_result != AWS_OP_SUCCESS) {
        assert(result == AWS_OP_ERR);
    }

    if (initial_item_allocation == 0) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(multiplication_result == AWS_OP_SUCCESS);
        assert(list.alloc == alloc);
        assert(list.current_size == expected_allocation_size);
        assert(list.length == 0);
        assert(list.item_size == item_size);
        assert(aws_array_list_capacity(&list) == initial_item_allocation);

        if (expected_allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
            assert(AWS_MEM_IS_WRITABLE(list.data, expected_allocation_size));
        }

        assert(list.current_size == 0 || list.data != NULL);
    } else {
        assert(list.alloc == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.data == NULL);
    }

    assert(aws_array_list_is_valid(&list));
}
