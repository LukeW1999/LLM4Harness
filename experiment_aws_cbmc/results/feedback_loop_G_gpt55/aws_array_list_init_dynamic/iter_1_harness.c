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

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_allocator *old_alloc_arg = alloc;
    size_t old_initial_item_allocation = initial_item_allocation;
    size_t old_item_size = item_size;

    size_t expected_allocation_size = 0;
    int multiplication_result = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation_size);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(multiplication_result == AWS_OP_SUCCESS);
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == expected_allocation_size);
        assert(list.current_size == initial_item_allocation * item_size);
        assert(aws_array_list_length(&list) == 0);
        assert(aws_array_list_capacity(&list) == initial_item_allocation);
        assert(list.current_size == 0 || list.data != NULL);
        if (expected_allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
            assert(AWS_MEM_IS_WRITABLE(list.data, expected_allocation_size));
        }
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.alloc == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.data == NULL);
        assert(multiplication_result != AWS_OP_SUCCESS || expected_allocation_size > 0);
    }

    assert(alloc == old_alloc_arg);
    assert(initial_item_allocation == old_initial_item_allocation);
    assert(item_size == old_item_size);
}
