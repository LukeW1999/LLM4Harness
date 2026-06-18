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
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t byte_offset = 0;
    __CPROVER_assume(aws_mul_size_checked(index, list.item_size, &byte_offset) == AWS_OP_SUCCESS);

    size_t expected_length_on_growth = 0;
    __CPROVER_assume(aws_add_size_checked(index, 1, &expected_length_on_growth) == AWS_OP_SUCCESS);

    size_t required_size = 0;
    __CPROVER_assume(aws_mul_size_checked(expected_length_on_growth, list.item_size, &required_size) == AWS_OP_SUCCESS);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);
    }

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (index < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index >= old.length) {
            assert(list.length == expected_length_on_growth);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= required_size);
        assert(list.current_size / list.item_size > index);
        assert(list.data != NULL);
        assert_bytes_match((const uint8_t *)list.data + byte_offset, val, list.item_size);

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        } else {
            assert(list.current_size >= old.current_size);
            if (index >= old_capacity) {
                assert(list.current_size > old.current_size);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);

        if (old.current_size > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
