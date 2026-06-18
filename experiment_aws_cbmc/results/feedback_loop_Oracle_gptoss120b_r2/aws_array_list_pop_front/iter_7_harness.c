#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INITIAL_ITEM_ALLOCATION 8U
#define MAX_ITEM_SIZE               32U

void aws_array_list_pop_front_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    struct aws_array_list list;
    int init_rv = aws_array_list_init(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(init_rv == AWS_OP_SUCCESS);

    /* Ensure the allocated buffer is initialized so reads are defined */
    memset(list.data, 0, list.current_size);

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    size_t max_len = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);
    list.length = len;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t item_size    = list.item_size;

    uint8_t *old_data = NULL;
    if (old_length > 0) {
        size_t copy_bytes = old_length * item_size;
        __CPROVER_assume(copy_bytes <= list.current_size);
        old_data = malloc(copy_bytes);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_assume(!__CPROVER_same_object(old_data, list.data));
        memcpy(old_data, list.data, copy_bytes);
    }

    int rv = aws_array_list_pop_front(&list);

    if (old_length > 0) {
        assert(rv == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(rv == AWS_ERROR_LIST_EMPTY);
        assert(aws_array_list_length(&list) == old_length);
    }

    assert(list.item_size == item_size);
    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.alloc == alloc);

    if (old_length > 0 && old_data != NULL) {
        size_t new_length = aws_array_list_length(&list);
        for (size_t i = 0; i < new_length; ++i) {
            assert(
                memcmp(
                    old_data + (i + 1) * item_size,
                    (uint8_t *)list.data + i * item_size,
                    item_size) == 0);
        }
    }
}
