/*
 * Harness for aws_array_list_get_at
 * Verifies that the function respects its return‑code contract and does not
 * modify the list structure.
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Stub for aws_raise_error so that the return value is deterministic */
int aws_raise_error(int err) {
    return err;
}

void aws_array_list_get_at_harness(void) {
    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Non‑deterministic item size (must be > 0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64); /* bound to keep model size reasonable */

    /* Non‑deterministic capacity (number of items that can be stored) */
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 32);

    /* Allocation size = capacity * item_size, checked for overflow */
    size_t allocation_size;
    __CPROVER_assume(!aws_mul_size_checked(capacity, item_size, &allocation_size));

    /* Allocate raw buffer for the list */
    uint8_t *raw_buffer = (uint8_t *)malloc(allocation_size);
    __CPROVER_assume(raw_buffer != NULL);

    /* Initialize the list */
    struct aws_array_list list;
    AWS_ZERO_STRUCT(list);
    list.alloc = allocator;
    list.item_size = item_size;
    list.current_size = allocation_size;
    list.data = raw_buffer;

    /* Non‑deterministic length (must be <= capacity) */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Fill the list with nondet data */
    for (size_t i = 0; i < length; ++i) {
        uint8_t *elem = (uint8_t *)list.data + i * item_size;
        for (size_t j = 0; j < item_size; ++j) {
            elem[j] = (uint8_t)__CPROVER_nondet_uint();
        }
    }

    /* Allocate destination buffer for get_at */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Snapshot list structure for post‑condition checks */
    struct aws_array_list list_before = list;

    /* Non‑deterministic index (allow out‑of‑bounds cases) */
    size_t index;
    __CPROVER_assume(index < capacity + 5);

    /* Call the function under test */
    int ret = aws_array_list_get_at(&list, val, index);

    /* Post‑condition: return value */
    if (index < list_before.length) {
        __CPROVER_assert(ret == AWS_OP_SUCCESS,
                         "aws_array_list_get_at should succeed when index < length");
    } else {
        __CPROVER_assert(ret == AWS_ERROR_INVALID_INDEX,
                         "aws_array_list_get_at should return INVALID_INDEX when index >= length");
    }

    /* Post‑condition: list invariants unchanged */
    __CPROVER_assert(list.length == list_before.length,
                     "list length must be unchanged");
    __CPROVER_assert(list.current_size == list_before.current_size,
                     "list current_size must be unchanged");
    __CPROVER_assert(list.item_size == list_before.item_size,
                     "list item_size must be unchanged");
    __CPROVER_assert(list.alloc == list_before.alloc,
                     "list allocator must be unchanged");
    __CPROVER_assert(list.data == list_before.data,
                     "list data pointer must be unchanged");

    /* Clean up */
    free(list.data);
    free(val);
}
