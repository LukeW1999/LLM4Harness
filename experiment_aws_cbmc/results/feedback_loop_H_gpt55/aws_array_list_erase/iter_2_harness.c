#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(capacity <= SIZE_MAX / item_size);

    size_t current_size = capacity * item_size;

    list.alloc = aws_default_allocator();
    list.data = malloc(current_size);
    __CPROVER_assume(list.data != NULL);
    list.current_size = current_size;
    list.item_size = item_size;

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    for (size_t i = 0; i < current_size; ++i) {
        ((uint8_t *)list.data)[i] = nondet_uint8_t();
    }

    assert(aws_array_list_is_valid(&list));
    assert(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    struct aws_array_list old = list;

    uint8_t *old_data = malloc(old.current_size);
    __CPROVER_assume(old_data != NULL);

    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = ((uint8_t *)old.data)[i];
    }

    size_t index = nondet_size_t();

    int result = aws_array_list_erase(&list, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (index < old.length) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(index < old.length);
        assert(old.length > 0);
        assert(list.length == old.length - 1);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        size_t prefix_bytes = index * old.item_size;
        size_t prefix_byte = nondet_size_t();
        assert(prefix_byte >= prefix_bytes ||
               ((uint8_t *)list.data)[prefix_byte] == old_data[prefix_byte]);

        size_t trailing_items = old.length - index - 1;
        size_t trailing_bytes = trailing_items * old.item_size;
        size_t trailing_byte = nondet_size_t();
        assert(trailing_byte >= trailing_bytes ||
               ((uint8_t *)list.data)[prefix_bytes + trailing_byte] ==
                   old_data[prefix_bytes + old.item_size + trailing_byte]);
    } else {
        assert(index >= old.length);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        size_t unchanged_byte = nondet_size_t();
        assert(unchanged_byte >= old.current_size ||
               ((uint8_t *)list.data)[unchanged_byte] == old_data[unchanged_byte]);

        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

    assert(aws_array_list_is_valid(&list));
    assert(aws_array_list_is_valid(&old));
    assert(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    assert(aws_array_list_is_bounded(&old, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    free(old_data);
    free(list.data);
}
