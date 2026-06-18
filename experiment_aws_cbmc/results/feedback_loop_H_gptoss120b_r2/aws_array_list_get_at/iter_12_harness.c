#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_get_at_harness() {
    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Assume the allocator's release function is usable */
    __CPROVER_assume(allocator->mem_release != 0);

    struct aws_array_list list;

    /* nondet item size, bounded */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* nondet capacity, bounded and non‑zero */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* initialize the list and assume success */
    int init_res = aws_array_list_init(&list, allocator, item_size, capacity);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(list.data != 0);               /* data must be allocated */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length, must be >0 to satisfy get_at preconditions */
    size_t length = nondet_size_t();
    __CPROVER_assume(length > 0);
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Fill the allocated region with nondet data */
    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < list.length * item_size; ++i) {
        data[i] = nondet_uint8_t();
    }

    /* nondet index within bounds */
    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < list.length);

    /* allocate destination buffer */
    uint8_t *val = (uint8_t *)malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    int result = aws_array_list_get_at(&list, val, idx);
    assert(result == AWS_OP_SUCCESS);

    /* verify the copied value matches the source */
    for (size_t i = 0; i < item_size; ++i) {
        assert(val[i] == data[idx * item_size + i]);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
