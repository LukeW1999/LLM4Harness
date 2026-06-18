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

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);

    size_t old_length_bytes = 0;
    int old_length_bytes_result = aws_mul_size_checked(old.length, old.item_size, &old_length_bytes);
    assert(old_length_bytes_result == AWS_OP_SUCCESS);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (old.alloc == NULL && index < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));

        size_t expected_length = 0;
        int add_result = aws_add_size_checked(index, 1, &expected_length);
        assert(add_result == AWS_OP_SUCCESS);

        if (index >= old.length) {
            assert(list.length == expected_length);
        } else {
            assert(list.length == old.length);
        }

        size_t offset = 0;
        int offset_result = aws_mul_size_checked(index, list.item_size, &offset);
        assert(offset_result == AWS_OP_SUCCESS);
        assert_bytes_match((const uint8_t *)list.data + offset, val, list.item_size);

        size_t new_capacity = aws_array_list_capacity(&list);
        assert(index < new_capacity);
        assert(list.length <= new_capacity);

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
            assert(index < old_capacity);
        }

        if (old.alloc != NULL && index >= old_capacity) {
            assert(list.current_size > old.current_size);
            assert(list.data != NULL);
        }

        if (old.alloc != NULL && old.current_size > 0 && old.current_size == old_length_bytes && index == old.length &&
            old.current_size <= ((size_t)-1) / 2) {
            assert(list.current_size >= old.current_size * 2);
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
