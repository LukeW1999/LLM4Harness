#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LIST_BYTES (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE)

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(initial_item_allocation <= ((size_t)-1) / item_size);

    uint8_t static_data[MAX_LIST_BYTES];

    if (nondet_bool()) {
        int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        aws_array_list_init_static(&list, static_data, initial_item_allocation, item_size);
    }

    size_t initial_length = nondet_size_t();
    __CPROVER_assume(initial_length <= initial_item_allocation);
    list.length = initial_length;

    assert(aws_array_list_is_valid(&list));

    size_t old_content_size = list.length * list.item_size;
    __CPROVER_assume(old_content_size <= MAX_LIST_BYTES);

    uint8_t old_data[MAX_LIST_BYTES];
    uint8_t val[MAX_ITEM_SIZE];
    uint8_t val_before[MAX_ITEM_SIZE];

    for (size_t i = 0; i < MAX_LIST_BYTES; ++i) {
        if (i < old_content_size) {
            ((uint8_t *)list.data)[i] = nondet_uint8_t();
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
        val_before[i] = val[i];
    }

    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;

    int result = aws_array_list_push_front(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        if (i < old.item_size) {
            assert(val[i] == val_before[i]);
        }
    }

    size_t required_size = (old.length + 1) * old.item_size;

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.current_size >= required_size);

        for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
            if (i < old.item_size) {
                assert(((uint8_t *)list.data)[i] == val_before[i]);
            }
        }

        for (size_t i = 0; i < MAX_LIST_BYTES; ++i) {
            if (i < old_content_size) {
                assert(((uint8_t *)list.data)[old.item_size + i] == old_data[i]);
            }
        }

        if (old.current_size >= required_size) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            assert(old.alloc != NULL);
            assert(list.current_size >= required_size);
        }
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        for (size_t i = 0; i < MAX_LIST_BYTES; ++i) {
            if (i < old_content_size) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }
}
