#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U
#define MAX_TOTAL_SIZE (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE)

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;

    /* nondet initialization of the list structure */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* bound the total allocated size to avoid huge mallocs */
    __CPROVER_assume(list.current_size <= MAX_TOTAL_SIZE);
    __CPROVER_assume(list.alloc <= MAX_TOTAL_SIZE);
    __CPROVER_assume(list.current_size <= list.alloc);

    /* allocate the underlying data buffer if needed */
    if (list.alloc > 0) {
        list.data = malloc(list.alloc);
        __CPROVER_assume(list.data);
    } else {
        list.data = NULL;
    }

    void *val;
    void *old_val = val; /* snapshot for error case */

    /* snapshot of the whole list structure */
    struct aws_array_list old_list = list;

    /* snapshot of the underlying data buffer */
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data);
        __CPROVER_assume(old_data != list.data); /* ensure no overlap */
        memcpy(old_data, list.data, list.current_size);
    }

    size_t index = __CPROVER_nondet_size_t();

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* ----- postconditions ----- */
    if (result == AWS_OP_SUCCESS) {
        assert(index < aws_array_list_length(&old_list));
        assert(val == (void *)((uint8_t *)old_list.data + old_list.item_size * index));
    } else {
        assert(index >= aws_array_list_length(&old_list));
        assert(val == old_val);
    }

    /* list invariants unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* memory not modified beyond the function's contract */
    if (old_data != NULL) {
        assert(memcmp(old_data, list.data, list.current_size) == 0);
        free(old_data);
    }

    return;
}
