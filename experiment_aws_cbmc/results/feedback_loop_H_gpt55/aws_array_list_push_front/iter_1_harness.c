#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length < SIZE_MAX);
    __CPROVER_assume(list.length + 1 <= SIZE_MAX / list.item_size);

    uint8_t val[MAX_ITEM_SIZE];
    uint8_t val_before[MAX_ITEM_SIZE];

    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }

    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    for (size_t i = 0; i < list.item_size; ++i) {
        val_before[i] = val[i];
    }

    struct aws_array_list old = list;

    uint8_t *old_data = NULL;
    if (old.current_size > 0) {
        old_data = malloc(old.current_size);
        __CPROVER_assume(old_data != NULL);
        for (size_t i = 0; i < old.current_size; ++i) {
            old_data[i] = ((uint8_t *)old.data)[i];
        }
    }

    int result = aws_array_list_push_front(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    for (size_t i = 0; i < old.item_size; ++i) {
        assert(val[i] == val_before[i]);
    }

    if (result == AWS_OP_SUCCESS) {
        size_t required_size = (old.length + 1) * old.item_size;
        size_t shifted_bytes = old.length * old.item_size;

        assert(list.length == old.length + 1);
        assert(list.current_size >= required_size);

        if (old.alloc == NULL || old.current_size >= required_size) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            assert(list.current_size >= old.current_size);
        }
