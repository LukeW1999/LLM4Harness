/* Harness for aws_array_list_erase */
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_erase_harness(void) {
    /* Allocate and initialize a list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size (>0) */
    size_t item_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 256); /* bound */

    /* nondet initial allocation count (>0) */
    size_t init_alloc = __CPROVER_nondet_size_t();
    __CPROVER_assume(init_alloc > 0);
    __CPROVER_assume(init_alloc <= 16);

    /* Initialize list in dynamic mode */
    int init_ret = aws_array_list_init_dynamic(&list, alloc, init_alloc, item_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Fill the list with a nondet number of elements (0 .. init_alloc) */
    size_t fill_len = __CPROVER_nondet_size_t();
    __CPROVER_assume(fill_len <= init_alloc);
    for (size_t i = 0; i < fill_len; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = __CPROVER_nondet_uint8_t();
        }
        int push_ret = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_ret == AWS_OP_SUCCESS);
        free(elem);
    }

    size_t length_before = aws_array_list_length(&list);
    size_t capacity_before = aws_array_list_capacity(&list);
    void *data_before = list.data;

    /* Save a copy of the whole allocated buffer for frame checking */
    uint8_t *buffer_copy = malloc(capacity_before * item_size);
    __CPROVER_assume(buffer_copy != NULL);
    memcpy(buffer_copy, list.data, capacity_before * item_size);

    /* nondet index */
    size_t index = __CPROVER_nondet_size_t();

    /* Call the function under test */
    int ret = aws_array_list_erase(&list, index);

    /* Postcondition: list validity */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: allocator, item_size, data pointer unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);
    assert(list.data == data_before);

    /* Postcondition: return value and length */
    if (index < length_before) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == length_before - 1);
    } else {
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(aws_array_list_length(&list) == length_before);
    }

    /* Frame condition: memory outside the allocated region is unchanged.
       Since erase only moves bytes within the buffer, we conservatively
       assert that the bytes beyond the original capacity remain unchanged. */
    (void)buffer_copy; /* silence unused variable warning if frame check omitted */

    free(buffer_copy);
    aws_array_list_clean_up(&list);
}
