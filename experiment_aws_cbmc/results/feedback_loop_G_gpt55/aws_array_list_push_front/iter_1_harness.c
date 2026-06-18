#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_ARRAY_LIST_BYTES (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE)

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.current_size <= MAX_ARRAY_LIST_BYTES);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.length < SIZE_MAX);
    __CPROVER_assume(list.length * list.item_size <= list.current_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_used_bytes = old.length * old.item_size;
    size_t expected_length = old.length + 1;
    size_t expected_used_bytes = old_used_bytes + old.item_size;

    __CPROVER_assume(old_used_bytes <= old.current_size);
    __CPROVER_assume(expected_used_bytes >= old_used_bytes);

    uint8_t old_data[MAX_ARRAY_LIST_BYTES];
    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = ((const uint8_t *)old.data)[i];
    }

    int result = aws_array_list_push_front(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (old.alloc == NULL && old.length == old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(old_length == old.length);
        assert(list.length == expected_length);
        assert(aws_array_list_length(&list) == expected_length);
        assert(list.current_size >= expected_used_bytes);
        assert(aws_array_list_capacity(&list) >= expected_length);
        assert(aws_array_list_capacity(&list) >= old_capacity);

        if (old.alloc == NULL || old.length < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        assert(AWS_MEM_IS_READABLE(list.data, expected_used_bytes));
        if (AWS_MEM_IS_READABLE(list.data, expected_used_bytes)) {
            const uint8_t *new_data = (const uint8_t *)list.data;
            const uint8_t *val_data = (const uint8_t *)val;

            for (size_t i = 0; i < old.item_size; ++i) {
                assert(new_data[i] == val_data[i]);
            }

            for (size_t i = 0; i < old_used_bytes; ++i) {
                assert(new_data[old.item_size + i] == old_data[i]);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_length(&list) == old_length);
        assert(aws_array_list_capacity(&list) == old_capacity);

        assert(AWS_MEM_IS_READABLE(list.data, old.current_size));
        if (AWS_MEM_IS_READABLE(list.data, old.current_size)) {
            const uint8_t *new_data = (const uint8_t *)list.data;

            for (size_t i = 0; i < old.current_size; ++i) {
                assert(new_data[i] == old_data[i]);
            }
        }
    }

    assert(aws_array_list_is_valid(&list));
}
