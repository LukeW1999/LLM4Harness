#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
int nondet_int(void);

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation and item size within bounds */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* initialize a dynamic list */
    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet number of elements to push (may be zero) */
    size_t pre_len = nondet_size_t();
    __CPROVER_assume(pre_len <= initial_item_allocation * 2U); /* allow some growth */

    /* fill the list with nondet data */
    uint8_t *elem = malloc(item_size);
    __CPROVER_assume(elem != NULL);
    for (size_t i = 0; i < pre_len; ++i) {
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = nondet_uint8_t();
        }
        (void)aws_array_list_push_back(&list, elem);
    }
    free(elem);

    /* capture old state */
    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    void *old_data      = list.data;
    size_t total_bytes  = old_capacity * list.item_size;
    uint8_t *old_buffer = malloc(total_bytes);
    __CPROVER_assume(old_buffer != NULL);
    memcpy(old_buffer, list.data, total_bytes);

    /* nondet index to erase */
    size_t index = nondet_size_t();

    /* call the function under verification */
    int ret = aws_array_list_erase(&list, index);

    /* post‑conditions */
    if (index >= old_length) {
        /* error case */
        assert(ret != AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length);
        assert(aws_array_list_capacity(&list) == old_capacity);
        assert(list.data == old_data);
        assert(0 == memcmp(old_buffer, list.data, total_bytes));
    } else {
        /* success case */
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
        assert(aws_array_list_capacity(&list) == old_capacity);
        assert(list.data == old_data);
        assert(aws_array_list_is_valid(&list));

        size_t item_sz = list.item_size;

        /* bytes before the erased element must be unchanged */
        if (index > 0) {
            assert(0 == memcmp(old_buffer,
                               (uint8_t *)list.data,
                               index * item_sz));
        }

        /* bytes after the new logical end must be unchanged */
        size_t after_start = (old_length - 1) * item_sz;
        if (after_start < total_bytes) {
            assert(0 == memcmp(old_buffer + after_start,
                               (uint8_t *)list.data + after_start,
                               total_bytes - after_start));
        }
    }

    free(old_buffer);
    aws_array_list_clean_up(&list);
    return 0;
}
