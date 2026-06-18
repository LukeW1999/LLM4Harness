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

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    struct store_byte_from_buffer old_data_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_data_byte);

    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array((const uint8_t *)val, old.item_size, &old_val_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    assert_byte_from_buffer_matches((const uint8_t *)val, &old_val_byte);

    if (index < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length;

        if (index < old.length) {
            expected_length = old.length;
        } else {
            int add_result = aws_add_size_checked(index, 1, &expected_length);
            assert(add_result == AWS_OP_SUCCESS);
        }

        assert(list.length == expected_length);
        assert(list.length > index);

        size_t item_offset;
        int mul_result = aws_mul_size_checked(index, list.item_size, &item_offset);
        assert(mul_result == AWS_OP_SUCCESS);
        assert(item_offset <= list.current_size);
        assert(list.item_size <= list.current_size - item_offset);
        assert_bytes_match((const uint8_t *)list.data + item_offset, val, list.item_size);

        if (index < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else if (index >= old_capacity) {
            assert(list.current_size > old.current_size);
            if (old.current_size == 0) {
                assert(list.current_size >= list.item_size);
            } else if (old.length == old_capacity && old.current_size <= SIZE_MAX / 2) {
                assert(list.current_size >= old.current_size * 2);
            }
        }

        size_t required_size;
        int required_size_result = aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(required_size_result == AWS_OP_SUCCESS);
        assert(list.current_size >= required_size);
        assert(list.data != NULL);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_data_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
