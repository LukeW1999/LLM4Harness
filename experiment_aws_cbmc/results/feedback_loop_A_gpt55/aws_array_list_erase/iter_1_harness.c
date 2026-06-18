#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());

    ensure_array_list_has_allocated_data_member(&list);

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    if (list.current_size > 0) {
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    }

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    uint8_t *old_data = NULL;
    if (old.current_size > 0) {
        old_data = malloc(old.current_size);
        __CPROVER_assume(old_data != NULL);

        for (size_t i = 0; i < old.current_size; ++i) {
            old_data[i] = ((uint8_t *)old.data)[i];
        }
    }

    const size_t old_capacity = aws_array_list_capacity(&old);
    const size_t old_length = aws_array_list_length(&old);

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        assert(index < old_length);
        assert(old_length > 0);

        assert(list.length == old.length - 1);
        assert(aws_array_list_length(&list) == old_length - 1);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == old_capacity);

        size_t removed_item_offset = index * old.item_size;
        size_t new_logical_size = list.length * list.item_size;

        for (size_t i = 0; i < removed_item_offset; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }

        for (size_t i = removed_item_offset; i < new_logical_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i + old.item_size]);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(index >= old_length);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_capacity(&list) == old_capacity);
        assert(aws_array_list_length(&list) == old_length);

        for (size_t i = 0; i < old.current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_is_valid(&list));
}
