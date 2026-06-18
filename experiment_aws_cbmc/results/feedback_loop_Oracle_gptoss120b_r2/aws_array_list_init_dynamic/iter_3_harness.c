#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024U
#define MAX_ITEM_SIZE               256U

size_t nondet_size_t(void);

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    AWS_ZERO_STRUCT(list);
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    struct aws_array_list old_list = list;

    int ret = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    size_t expected_allocation = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation);

    if (overflow) {
        assert(ret == AWS_OP_ERR);
    } else {
        if (expected_allocation > 0) {
            if (list.data == NULL) {
                assert(ret == AWS_OP_ERR);
            } else {
                assert(ret == AWS_OP_SUCCESS);
            }
        } else {
            assert(ret == AWS_OP_SUCCESS);
        }
    }

    if (ret == AWS_OP_SUCCESS) {
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
        if (expected_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            assert(list.current_size == expected_allocation);
            assert(list.data != NULL);
        }
    } else {
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size || ret == AWS_OP_SUCCESS);
    assert(list.data == old_list.data || ret == AWS_OP_SUCCESS);
}
