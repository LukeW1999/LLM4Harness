#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void **val = malloc(sizeof(void *));
    __CPROVER_assume(val);

    size_t index;

    /* Save original list state for frame condition checks */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* 1. Return value / error code correctness */
    if (index < old_length) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
        /* val should point into the data buffer at the correct offset */
        assert(*val == (void *)((uint8_t *)old_data + (old_item_size * index)));
    } else {
        /* Should fail with invalid index error */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }

    /* 2. Output buffer length/capacity invariants - list structure is unchanged */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);

    /* 3. Frame conditions - list data pointer not modified */
    assert(list.data == old_data);
}
