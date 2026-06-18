#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    uint8_t raw_array[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    uint8_t value[MAX_ITEM_SIZE];

    size_t item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t();
    size_t initial_length = nondet_size_t();
    size_t index = nondet_size_t();
    bool use_dynamic_allocation = nondet_bool();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(initial_item_allocation <= SIZE_MAX / item_size);
    __CPROVER_assume(initial_length <= initial_item_allocation);
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);
    __CPROVER_assume(index + 1 <= SIZE_MAX / item_size);

    __CPROVER_havoc_object(raw_array);
    __CPROVER_havoc_object(value);

    assert(item_size > 0);
    assert(item_size <= MAX_ITEM_SIZE);
    assert(initial_item_allocation > 0);
    assert(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    assert(initial_length <= initial_item_allocation);
    assert(AWS_MEM_IS_READABLE(value, item_size));

    if (use_dynamic_allocation) {
        struct aws_allocator *allocator = aws_default_allocator();
        assert(allocator != NULL);

        int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);
    }

    for (size_t i = 0; i < list.current_size; ++i) {
        ((uint8_t *)list.data)[i] = nondet_uint8_t();
    }

    list.length = initial_length;

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == item_size);
    assert(list.length == initial_length);
    assert(aws_array_list_capacity(&list) == initial_item_allocation);
    assert(AWS_MEM_IS_READABLE(value, list.item_size));

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);
    size_t required_size = (index + 1) * old.item_size;
    size_t target_offset = index * old.item_size;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, value, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.length == (index >= old.length ? index + 1 : old.length));
        assert(list.length >= old.length);
        assert(list.current_size >= required_size);
        assert(aws_array_list_capacity(&list) > index);
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        assert_bytes_match((const uint8_t *)list.data + target_offset, value, list.item_size);

        if (index < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }
    } else {
        assert(index >= old_capacity);
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (old.alloc == NULL) {
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));
}
