#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t *old_data = NULL;
    if (old.current_size > 0) {
        old_data = malloc(old.current_size);
        __CPROVER_assume(old_data != NULL);
        for (size_t i = 0; i < old.current_size; ++i) {
            old_data[i] = ((uint8_t *)old.data)[i];
        }
    }

    int result = aws_array_list_pop_front(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        size_t shifted_bytes = list.length * list.item_size;
        if (shifted_bytes > 0) {
            assert_bytes_match((const uint8_t *)list.data, old_data + old.item_size, shifted_bytes);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(old.length == 0);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        if (old.current_size > 0) {
            assert_bytes_match((const uint8_t *)list.data, old_data, old.current_size);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));
    assert(aws_array_list_is_valid(&list));
}
