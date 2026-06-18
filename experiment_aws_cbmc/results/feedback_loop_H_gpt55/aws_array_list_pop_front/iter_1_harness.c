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
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);
    __CPROVER_assume(list.current_size == 0 || AWS_MEM_IS_READABLE(list.data, list.current_size));
    __CPROVER_assume(list.current_size == 0 || AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    struct aws_array_list old = list;

    uint8_t *old_data = malloc(old.current_size);
    __CPROVER_assume(old.current_size == 0 || old_data != NULL);

    uint8_t *list_data_before = (uint8_t *)list.data;
    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = list_data_before[i];
    }

    int result = aws_array_list_pop_front(&list);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        uint8_t *list_data_after = (uint8_t *)list.data;
        size_t shifted_bytes = list.length * list.item_size;
        for (size_t i = 0; i < shifted_bytes; ++i) {
            assert(list_data_after[i] == old_data[i + old.item_size]);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(old.length == 0);
        assert(aws_last_error() == AWS_ERROR_LIST_EMPTY);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        uint8_t *list_data_after = (uint8_t *)list.data;
        for (size_t i = 0; i < old.current_size; ++i) {
            assert(list_data_after[i] == old_data[i]);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));
    assert(aws_array_list_is_valid(&list));
}
