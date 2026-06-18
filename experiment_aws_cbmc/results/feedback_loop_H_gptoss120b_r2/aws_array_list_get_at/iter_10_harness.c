#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_get_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(capacity > 0);

    aws_array_list_init(&list, allocator, item_size, capacity);

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Fill the list with nondet data */
    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < list.length * item_size; ++i) {
        data[i] = nondet_uint8_t();
    }

    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < list.length);

    uint8_t *val = (uint8_t *)malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    int result = aws_array_list_get_at(&list, val, idx);

    assert(result == AWS_OP_SUCCESS);
    for (size_t i = 0; i < item_size; ++i) {
        assert(val[i] == data[idx * item_size + i]);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
