#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data && old.current_size > 0) {
        save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);
    }

    size_t expected_length = 0;
    int expected_length_err = aws_add_size_checked(index, 1, &expected_length);

    size_t required_size = 0;
    int required_size_err = AWS_OP_ERR;
    if (expected_length_err == AWS_OP_SUCCESS) {
        required_size_err = aws_mul_size_checked(expected_length, old.item_size, &required_size);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.alloc == NULL && index >= old.current_size / old.item_size) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(expected_length_err == AWS_OP_SUCCESS);
        assert(required_size_err == AWS_OP_SUCCESS);

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index >= old.length) {
            assert(list.length == expected_length);
        } else {
            assert(list.length == old.length);
        }

        assert(index < list.length);
        assert(list.current_size >= required_size);

        if (old.alloc == NULL) {
            assert(index < old.current_size / old.item_size);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            if (old.current_size < required_size) {
                assert(list.current_size > old.current_size);
            } else {
                assert(list.current_size == old.current_size);
                assert(list.data == old.data);
            }
        }

        size_t byte_offset = 0;
        int byte_offset_err = aws_mul_size_checked(index, list.item_size, &byte_offset);
        assert(byte_offset_err == AWS_OP_SUCCESS);
        if (byte_offset_err == AWS_OP_SUCCESS) {
            assert_bytes_match((const uint8_t *)list.data + byte_offset, val, list.item_size);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);

        if (old.data && old.current_size > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
