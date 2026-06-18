#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum sizes for the proof harness */
#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               16U

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialize the list in dynamic mode with nondeterministic parameters */
    size_t init_allocation = nondet_size_t();
    size_t item_size       = nondet_size_t();

    __CPROVER_assume(init_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Ensure the init call succeeds – if it fails we abort the harness */
    int init_result = aws_array_list_init_dynamic(&list, alloc, init_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Make the list valid and bounded as required by the specification */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of the list state before the call */
    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_sz  = list.item_size;
    void  *old_data     = list.data;

    /* Copy the entire underlying buffer for frame condition checks */
    uint8_t *old_buffer = NULL;
    if (old_data != NULL && old_capacity > 0) {
        old_buffer = malloc(old_capacity * old_item_sz);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, old_data, old_capacity * old_item_sz);
    }

    /* Nondeterministic index to erase */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int rv = aws_array_list_erase(&list, index);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks */
    /* -------------------------------------------------------------------- */

    /* 1. Return value / error code correctness */
    if (index >= old_length) {
        assert(rv == aws_raise_error(AWS_ERROR_INVALID_INDEX));
    } else {
        assert(rv == AWS_OP_SUCCESS);
    }

    /* 2. Length invariants */
    if (index < old_length) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }

    /* Capacity must remain unchanged */
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* Data pointer must remain unchanged */
    assert(list.data == old_data);

    /* 3. Frame condition: memory outside the used portion of the buffer is unchanged */
    if (old_buffer != NULL && old_data != NULL) {
        size_t used_bytes_before = old_length * old_item_sz;
        size_t total_bytes       = old_capacity * old_item_sz;

        /* Bytes beyond the previously used region must be identical */
        for (size_t i = used_bytes_before; i < total_bytes; ++i) {
            assert(((uint8_t *)old_data)[i] == old_buffer[i]);
        }

        free(old_buffer);
    }

    /* Clean up the list */
    aws_array_list_clean_up(&list);
    return 0;
}
