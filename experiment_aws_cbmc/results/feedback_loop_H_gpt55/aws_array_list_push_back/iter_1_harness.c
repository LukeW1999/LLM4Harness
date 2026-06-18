#include <aws/common/byte_buf.h>
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

    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    size_t old_content_size = 0;
    int old_content_size_result = aws_mul_size_checked(old.length, old.item_size, &old_content_size);
    __CPROVER_assume(old_content_size_result == AWS_OP_SUCCESS);

    size_t saved_byte_index = nondet_size_t();
    uint8_t saved_byte = 0;
    bool saved_used_byte = old_content_size > 0;
    if (saved_used_byte) {
        __CPROVER_assume(saved_byte_index < old_content_size);
        saved_byte = ((uint8_t *)old.data)[saved_byte_index];
    }

    struct store_byte_from_buffer old_byte;
    save_byte_from_array(old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (index < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (old.alloc == NULL) {
        if (index >= old_capacity) {
            assert(result == AWS_OP_ERR);
        } else {
            assert(result == AWS_OP_SUCCESS);
        }
    }

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = 0;
        int add_result = aws_add_size_checked(index, 1, &expected_length);
        assert(add_result == AWS_OP_SUCCESS);

        size_t required_size = 0;
        int required_size_result = aws_mul_size_checked(expected_length, list.item_size, &required_size);
        assert(required_size_result == AWS_OP_SUCCESS);

        size_t element_offset = 0;
        int offset_result = aws_mul_size_checked(index, list.item_size, &element_offset);
        assert(offset_result == AWS_OP_SUCCESS);

        assert(list.length > index);
        assert(list.current_size >= required_size);
        assert(list.data != NULL);

        if (index >= old.length) {
            assert(list.length == expected_length);
        } else {
            assert(list.length == old.length);
        }

        assert_bytes_match((const uint8_t *)list.data + element_offset, val, list.item_size);

        if (saved_used_byte) {
            if (saved_byte_index < element_offset || saved_byte_index >= required_size) {
                assert(((uint8_t *)list.data)[saved_byte_index] == saved_byte);
            }
        }

        if (index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc != NULL && index >= old_capacity) {
            assert(list.current_size >= required_size);
            assert(list.current_size > old.current_size || old.current_size == 0);

            if (old.current_size > 0 && old.current_size % old.item_size == 0) {
                size_t doubled_size = 0;
                int doubled_result = aws_mul_size_checked(old.current_size, 2, &doubled_size);
                assert(doubled_result == AWS_OP_SUCCESS);
                assert(list.current_size >= doubled_size);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
