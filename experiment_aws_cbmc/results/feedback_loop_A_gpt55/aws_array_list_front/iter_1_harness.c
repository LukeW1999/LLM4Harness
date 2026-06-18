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
    __CPROVER_assume(index < SIZE_MAX);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    struct store_byte_from_buffer old_data_byte;
    if (old.current_size > 0) {
        save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_data_byte);
    }

    struct store_byte_from_buffer val_byte;
    save_byte_from_array((const uint8_t *)val, list.item_size, &val_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.alloc == NULL) {
        if (index >= old_capacity) {
            assert(result == AWS_OP_ERR);
        } else {
            assert(result == AWS_OP_SUCCESS);
        }
    }

    if (result == AWS_OP_SUCCESS) {
        size_t required_item_count = index + 1;
        size_t required_size = 0;
        int required_size_overflow = aws_mul_size_checked(required_item_count, list.item_size, &required_size);

        assert(required_size_overflow == AWS_OP_SUCCESS);
        assert(list.data != NULL);
        assert(list.current_size >= required_size);
        assert(list.current_size >= old.current_size);

        if (list.data != NULL && required_size_overflow == AWS_OP_SUCCESS && list.current_size >= required_size) {
            assert_bytes_match((const uint8_t *)list.data + (index * list.item_size), val, list.item_size);
        } else {
            assert(false);
        }

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (old.alloc == NULL || index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
        }
    } else {
        assert(result == AWS_OP_ERR);

        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);

        if (old.current_size > 0 && list.data == old.data && list.current_size == old.current_size) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_data_byte);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert_byte_from_buffer_matches((const uint8_t *)val, &val_byte);
    assert(aws_array_list_is_valid(&list));
}
