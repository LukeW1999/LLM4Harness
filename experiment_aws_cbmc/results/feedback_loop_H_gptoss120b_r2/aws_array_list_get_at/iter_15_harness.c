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

    /* Assume allocator functions are usable */
    __CPROVER_assume(allocator->mem_acquire != 0);
    __CPROVER_assume(allocator->mem_release != 0);

    struct aws_array_list list;

    /* nondet item size, bounded and non‑zero */
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
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length, bounded by capacity, keep non‑zero to guarantee data is allocated */
    size_t length = nondet_size_t();
    __CPROVER_assume(length > 0);
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* nondet index within bounds */
    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < list.length);

    /* destination buffer allocated separately to avoid overlap */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != 0);

    int result = aws_array_list_get_at(&list, val, idx);
    assert(result == AWS_OP_SUCCESS);

    /* verify the copied value matches the source */
    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < item_size; ++i) {
        assert(val[i] == data[idx * item_size + i]);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
