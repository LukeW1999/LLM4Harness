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

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    size_t index_plus_one = 0;
    __CPROVER_assume(aws_add_size_checked(index, 1, &index_plus_one) == AWS_OP_SUCCESS);

    size_t required_size = 0;
    __CPROVER_assume(aws_mul_size_checked(index_plus_one, list.item_size, &required_size) == AWS_OP_SUCCESS);

    size_t target_begin = 0;
    __CPROVER_assume(aws_mul_size_checked(index, list.item_size, &target_begin) == AWS_OP_SUCCESS);

    size_t target_end = 0;
    __CPROVER_assume(aws_add_size_checked(target_begin, list.item_size, &target_end) == AWS_OP_SUCCESS);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;

    size_t old_logical_size = 0;
    int old_logical_size_result = aws_mul_size_checked(old.length, old.item_size, &old_logical_size);
    assert(old_logical_size_result == AWS_OP_SUCCESS);

    uint8_t *old_contents = NULL;
    if (old.current_size > 0) {
        assert(old.data != NULL);
        old_contents = malloc(old.current_size);
        __CPROVER_assume(old_contents != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_contents, old.current_size));

        for (size_t i = 0; i < old.current_size; ++i) {
            old_contents[i] = ((uint8_t *)old.data)[i];
        }
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index >= old.length) {
            assert(list.length == index_plus_one);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= required_size);
        assert(list.data != NULL);
        assert_bytes_match((const uint8_t *)list.data + target_begin, val, list.item_size);

        if (required_size <= old.current_size || old.alloc == NULL) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            assert(list.current_size > old.current_size);
        }

        if (old.alloc != NULL && old.current_size > 0 && old_logical_size == old.current_size && index >= old.length) {
            size_t doubled_size = 0;
            if (aws_mul_size_checked(old.current_size, 2, &doubled_size) == AWS_OP_SUCCESS) {
                assert(list.current_size >= doubled_size);
            }
        }

        for (size_t i = 0; i < old_logical_size; ++i) {
            if (i < target_begin || i >= target_end) {
                assert(((uint8_t *)list.data)[i] == old_contents[i]);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);

        for (size_t i = 0; i < old.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_contents[i]);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
