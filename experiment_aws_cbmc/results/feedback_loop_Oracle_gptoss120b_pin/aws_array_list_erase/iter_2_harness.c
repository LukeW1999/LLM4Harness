#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic initial allocation and item size within bounds */
    size_t init_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(init_allocation <= 10U);
    __CPROVER_assume(item_size > 0 && item_size <= 16U);

    /* Initialize list in dynamic mode */
    int init_res = aws_array_list_init_dynamic(&list, alloc, init_allocation, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_bounded(&list, 10U, 16U));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Populate the list with a nondeterministic number of elements (0..capacity) */
    size_t max_push = aws_array_list_capacity(&list);
    size_t push_count = nondet_size_t();
    __CPROVER_assume(push_count <= max_push);
    for (size_t i = 0; i < push_count; ++i) {
        uint8_t *elem = malloc(item_size);
        __CPROVER_assume(elem != NULL);
        /* Fill element with nondeterministic data */
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = nondet_uint8_t();
        }
        int push_res = aws_array_list_push_back(&list, elem);
        __CPROVER_assume(push_res == AWS_OP_SUCCESS);
        free(elem);
    }

    /* Snapshot before erase */
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_sz = list.item_size;
    void *old_data = list.data;

    uint8_t *old_buffer = NULL;
    if (old_data != NULL && old_capacity > 0) {
        old_buffer = malloc(old_capacity * old_item_sz);
        __CPROVER_assume(old_buffer != NULL);
        aws_memcpy(old_buffer, old_data, old_capacity * old_item_sz);
    }

    /* Nondeterministic index to erase */
    size_t index = nondet_size_t();

    /* Call function under verification */
    int rv = aws_array_list_erase(&list, index);

    /* Post‑condition checks */
    if (index >= old_length) {
        assert(rv == aws_raise_error(AWS_ERROR_INVALID_INDEX));
    } else {
        assert(rv == AWS_OP_SUCCESS);
    }

    if (index < old_length) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }

    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.data == old_data);

    if (old_buffer != NULL && old_data != NULL) {
        size_t used_bytes_before = old_length * old_item_sz;
        size_t total_bytes = old_capacity * old_item_sz;
        for (size_t i = used_bytes_before; i < total_bytes; ++i) {
            assert(((uint8_t *)old_data)[i] == old_buffer[i]);
        }
        free(old_buffer);
    }

    aws_array_list_clean_up(&list);
}
