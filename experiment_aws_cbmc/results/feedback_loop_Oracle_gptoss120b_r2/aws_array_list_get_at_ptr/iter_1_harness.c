#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* list under test */
    struct aws_array_list list;
    /* initialize list with nondeterministic but bounded parameters */
    make_aws_array_list(&list, aws_default_allocator(), MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* nondeterministic index */
    size_t index;

    /* output pointer */
    void *out;
    void **val = &out;

    /* preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val);

    /* snapshot of list state */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data != NULL) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data);
        memcpy(old_data, list.data, list.current_size);
    }

    /* call the function under verification */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* -------------------------------------------------------------------- */
    /* Postcondition 1: return value / error code correctness                */
    if (index < old_list.length) {
        __CPROVER_assert(result == AWS_OP_SUCCESS,
                         "aws_array_list_get_at_ptr returns success when index is in range");
        __CPROVER_assert(*val == (void *)((uint8_t *)list.data + list.item_size * index),
                         "output pointer points to the correct element");
    } else {
        __CPROVER_assert(result != AWS_OP_SUCCESS,
                         "aws_array_list_get_at_ptr returns error when index is out of range");
        __CPROVER_assert(aws_last_error() == AWS_ERROR_INVALID_INDEX,
                         "error code is AWS_ERROR_INVALID_INDEX on out‑of‑range index");
    }

    /* -------------------------------------------------------------------- */
    /* Postcondition 2: length / capacity invariants                         */
    __CPROVER_assert(list.length == old_list.length,
                     "list length unchanged");
    __CPROVER_assert(list.current_size == old_list.current_size,
                     "list current_size unchanged");
    __CPROVER_assert(list.item_size == old_list.item_size,
                     "list item_size unchanged");
    __CPROVER_assert(list.alloc == old_list.alloc,
                     "list allocator unchanged");
    __CPROVER_assert(list.data == old_list.data,
                     "list data pointer unchanged");

    /* -------------------------------------------------------------------- */
    /* Postcondition 3: frame conditions (memory not modified)               */
    if (old_data != NULL) {
        __CPROVER_assert(memcmp(old_data, list.data, list.current_size) == 0,
                         "list data buffer unchanged");
    }

    return 0;
}
