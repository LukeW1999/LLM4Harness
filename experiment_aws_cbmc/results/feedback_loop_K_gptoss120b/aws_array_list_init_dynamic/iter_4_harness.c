#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= 10);
    __CPROVER_assume(item_size <= 10);

    int ret = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    assert(list.item_size == item_size);
    assert(list.alloc == alloc);
    assert(list.length == 0);

    size_t allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);

    if (overflow) {
        assert(ret == AWS_OP_ERR);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    } else {
        if (allocation_size == 0) {
            assert(ret == AWS_OP_SUCCESS);
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            if (ret == AWS_OP_SUCCESS) {
                assert(list.current_size == allocation_size);
                assert(list.data != NULL);
            } else {
                assert(ret == AWS_OP_ERR);
                assert(list.current_size == 0);
                assert(list.data == NULL);
            }
        }
    }
}
