#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_ARRAY_LIST_STORAGE_SIZE (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE)

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.current_size <= MAX_ARRAY_LIST_STORAGE_SIZE);

    struct aws_array_list old = list;

    uint8_t old_data[MAX_ARRAY_LIST_STORAGE_SIZE + 1];
    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = data[i];
    }

    int result = aws_array_list_pop_front(&list);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);

        size_t shifted_size = (old.length - 1) * old.item_size;
        if (shifted_size > 0) {
            assert_bytes_match((uint8_t *)list.data, old_data + old.item_size, shifted_size);
        }
    } else {
        assert(old.length == 0);
        assert(list.length == old.length);

        if (old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data, old_data, old.current_size);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
