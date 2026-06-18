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

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size > 0);
    assert(list.item_size <= MAX_ITEM_SIZE);

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    const size_t old_capacity = old.current_size / old.item_size;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(aws_array_list_is_valid(&list));

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        size_t index_plus_one = 0;
        int add_result = aws_add_size_checked(index, 1, &index_plus_one);
        assert(add_result == AWS_OP_SUCCESS);

        if (index >= old.length) {
            assert(list.length == index_plus_one);
        } else {
            assert(list.length == old.length);
        }

        assert(list.length >= old.length);
        assert(list.length > index);

        size_t item_offset = 0;
        int offset_result = aws_mul_size_checked(index, list.item_size, &item_offset);
        assert(offset_result == AWS_OP_SUCCESS);

        size_t indexed_item_end = 0;
        int end_result = aws_add_size_checked(item_offset, list.item_size, &indexed_item_end);
        assert(end_result == AWS_OP_SUCCESS);
        assert(list.current_size >= indexed_item_end);

        size_t required_size = 0;
        int required_result = aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(required_result == AWS_OP_SUCCESS);
        assert(list.current_size >= required_size);

        assert(list.data != NULL);
        assert_bytes_match((const uint8_t *)list.data + item_offset, val, list.item_size);

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        } else {
            if (index < old_capacity) {
                assert(list.data == old.data);
                assert(list.current_size == old.current_size);
            } else {
                assert(list.current_size > old.current_size);

                if (old.current_size > 0) {
                    size_t doubled_size = 0;
                    int double_result = aws_mul_size_checked(old.current_size, 2, &doubled_size);
                    if (double_result == AWS_OP_SUCCESS) {
                        assert(list.current_size >= doubled_size);
                    }
                }
            }
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

    assert(aws_array_list_is_valid(&list));
}
