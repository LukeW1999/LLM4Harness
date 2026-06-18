#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(capacity <= SIZE_MAX / item_size);

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    bool use_allocator = nondet_bool();

    list.alloc = use_allocator ? aws_default_allocator() : NULL;
    list.current_size = capacity * item_size;
    list.length = length;
    list.item_size = item_size;

    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    assert(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);
    __CPROVER_assume(index + 1 <= SIZE_MAX / item_size);

    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length > index);
        assert(list.length >= old.length);
        assert(aws_array_list_capacity(&list) > index);
        assert(list.current_size >= old.current_size);

        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        assert_bytes_match((const uint8_t *)list.data + (list.item_size * index), val, list.item_size);

        if (index < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        assert(aws_array_list_is_valid(&list));
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (old.alloc == NULL && index < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
