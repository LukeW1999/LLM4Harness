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

    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t target_start = 0;
    size_t target_end = 0;
    __CPROVER_assume(aws_mul_size_checked(index, list.item_size, &target_start) == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_add_size_checked(target_start, list.item_size, &target_end) == AWS_OP_SUCCESS);

    size_t probe = nondet_size_t();
    uint8_t old_probe_byte = 0;
    if (list.current_size > 0) {
        __CPROVER_assume(probe < list.current_size);
        old_probe_byte = ((uint8_t *)list.data)[probe];
    }

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.length > index);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= old.current_size);
        assert(list.data != NULL);

        size_t byte_offset = 0;
        int mul_result = aws_mul_size_checked(index, list.item_size, &byte_offset);
        assert(mul_result == AWS_OP_SUCCESS);
        assert_bytes_match((uint8_t *)list.data + byte_offset, val, list.item_size);

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
            assert(index < old_capacity);
        }

        if (old.alloc != NULL && index < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc != NULL && index >= old_capacity) {
            assert(list.current_size > old.current_size);
        }

        if (old.current_size > 0 && probe < list.current_size) {
            if (probe < target_start || probe >= target_end) {
                assert(((uint8_t *)list.data)[probe] == old_probe_byte);
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

    if (old.alloc == NULL) {
        if (index < old_capacity) {
            assert(result == AWS_OP_SUCCESS);
        } else {
            assert(result == AWS_OP_ERR);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
