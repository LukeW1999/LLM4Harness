#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t required_length = 0;
    int required_length_valid = aws_add_size_checked(index, 1, &required_length);

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(required_length_valid == AWS_OP_SUCCESS);

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (required_length_valid == AWS_OP_SUCCESS) {
            if (index >= old.length) {
                assert(list.length == required_length);
            } else {
                assert(list.length == old.length);
            }

            size_t required_size = 0;
            int required_size_valid = aws_mul_size_checked(required_length, list.item_size, &required_size);
            assert(required_size_valid == AWS_OP_SUCCESS);
            if (required_size_valid == AWS_OP_SUCCESS) {
                assert(list.current_size >= required_size);
            }
        }

        size_t byte_offset = 0;
        int byte_offset_valid = aws_mul_size_checked(index, list.item_size, &byte_offset);
        assert(byte_offset_valid == AWS_OP_SUCCESS);
        if (byte_offset_valid == AWS_OP_SUCCESS) {
            assert_bytes_match((const uint8_t *)list.data + byte_offset, val, list.item_size);
        }

        if (index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        } else if (index >= old_capacity) {
            if (old_capacity > 0) {
                size_t doubled_size = 0;
                int doubled_size_valid = aws_mul_size_checked(old.current_size, 2, &doubled_size);
                assert(doubled_size_valid == AWS_OP_SUCCESS);
                if (doubled_size_valid == AWS_OP_SUCCESS) {
                    assert(list.current_size >= doubled_size);
                }
            } else {
                assert(list.current_size >= list.item_size);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
