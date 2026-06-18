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
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    size_t old_capacity = old.current_size / old.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        assert(list.length > index);

        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            size_t expected_length = 0;
            assert(aws_add_size_checked(index, 1, &expected_length) == AWS_OP_SUCCESS);
            assert(list.length == expected_length);
        }

        size_t byte_offset = 0;
        assert(aws_mul_size_checked(index, list.item_size, &byte_offset) == AWS_OP_SUCCESS);

        size_t write_end = 0;
        assert(aws_add_size_checked(byte_offset, list.item_size, &write_end) == AWS_OP_SUCCESS);
        assert(write_end <= list.current_size);

        assert_bytes_match((const uint8_t *)list.data + byte_offset, val, list.item_size);

        assert(list.current_size >= old.current_size);

        if (index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc != NULL && index >= old_capacity) {
            assert(list.current_size > old.current_size);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);

        assert_array_list_equivalence(&list, &old, &old_byte);

        if (old.alloc == NULL) {
            assert(index >= old_capacity);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
