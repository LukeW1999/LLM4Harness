#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef NDEBUG
#    undef NDEBUG
#endif
#include <assert.h>

#define HARNESS_MAX_ITEM_SIZE 4
#define HARNESS_MAX_CAPACITY 4

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    size_t capacity = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= HARNESS_MAX_ITEM_SIZE);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= HARNESS_MAX_CAPACITY);
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(capacity <= SIZE_MAX / item_size);

    size_t current_size = capacity * item_size;

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    uint8_t backing_store[HARNESS_MAX_CAPACITY * HARNESS_MAX_ITEM_SIZE];
    uint8_t old_data[HARNESS_MAX_CAPACITY * HARNESS_MAX_ITEM_SIZE];

    for (size_t i = 0; i < sizeof(backing_store); ++i) {
        backing_store[i] = (uint8_t)nondet_size_t();
        old_data[i] = backing_store[i];
    }

    list.alloc = aws_default_allocator();
    list.data = backing_store;
    list.current_size = current_size;
    list.length = length;
    list.item_size = item_size;

    assert(list.alloc == aws_default_allocator());
    assert(list.data == backing_store);
    assert(list.item_size > 0);
    assert(list.current_size == capacity * item_size);
    assert(list.length <= capacity);
    assert(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);
    size_t old_length = aws_array_list_length(&old);

    bool use_valid_index = nondet_size_t() != 0;
    size_t index;

    if (use_valid_index) {
        __CPROVER_assume(old_length > 0);
        index = nondet_size_t();
        __CPROVER_assume(index < old_length);
        assert(index < old_length);
    } else {
        index = old_length;
        assert(index >= old_length);
    }

    int result = aws_array_list_erase(&list, index);

    if (use_valid_index) {
        assert(result == AWS_OP_SUCCESS);
        assert(index < old_length);
        assert(old_length > 0);

        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length - 1);

        assert(aws_array_list_capacity(&list) == old_capacity);
        assert(aws_array_list_length(&list) == old_length - 1);

        size_t removed_item_offset = index * old.item_size;
        size_t new_logical_size = list.length * list.item_size;

        for (size_t i = 0; i < removed_item_offset; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }

        for (size_t i = removed_item_offset; i < new_logical_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i + old.item_size]);
        }

        for (size_t i = new_logical_size; i < old.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(index >= old_length);

        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);

        assert(aws_array_list_capacity(&list) == old_capacity);
        assert(aws_array_list_length(&list) == old_length);

        for (size_t i = 0; i < old.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
