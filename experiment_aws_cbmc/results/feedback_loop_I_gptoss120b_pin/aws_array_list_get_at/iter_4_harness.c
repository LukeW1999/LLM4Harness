#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);

    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(item_sz == 0 || index <= SIZE_MAX / item_sz);

    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.current_size > 0) {
        old_data_copy = malloc(old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        for (size_t i = 0; i < old.current_size; ++i) {
            old_data_copy[i] = ((uint8_t *)old.data)[i];
        }
    }

    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (old.current_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data,
                old_data_copy,
                old.current_size);
        } else {
            assert(list.data == old.data);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    if (old_data_copy) {
        free(old_data_copy);
    }
}
