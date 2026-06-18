/*
Contract for aws_array_list_get_at

Preconditions:
- list is a valid aws_array_list (aws_array_list_is_valid(list) == true)
- list->item_size > 0
- list->data points to a writable memory region of size list->current_size (if current_size > 0)
- list->length <= list->current_size / list->item_size (i.e., length fits in allocated buffer)
- val is a non‑NULL pointer to a writable memory region of at least list->item_size bytes
- index is a size_t value (no additional restriction)

Postconditions (validity):
- The function returns AWS_OP_SUCCESS iff index < list->length, otherwise it returns AWS_ERROR_INVALID_INDEX.
- In the success case, the memory pointed to by val contains a byte‑wise copy of the element stored at list->data + index * list->item_size.

Postconditions (length / capacity invariants):
- list->length, list->current_size, list->item_size, and list->alloc are unchanged by the call.

Postconditions (frame):
- No memory outside of the element copied into *val is modified.
- The contents of list->data are unchanged.
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

    /* Initialize the list as a dynamic array */
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

    /* Snapshot state for post‑condition checks */
    struct aws_array_list list_before = list;
    uint8_t *data_before = (uint8_t *)malloc(allocation_size);
    __CPROVER_assume(data_before != NULL);
    memcpy(data_before, list.data, allocation_size);
    uint8_t *val_before = (uint8_t *)malloc(item_size);
    __CPROVER_assume(val_before != NULL);
    memcpy(val_before, val, item_size);

    /* Non‑deterministic index */
    size_t index;
    __CPROVER_assume(index < capacity + 5); /* allow out‑of‑bounds cases */

    /* Call the function under test */
    int ret = aws_array_list_get_at(&list, val, index);

    /* Post‑condition: return value */
    if (index < list_before.length) {
        __CPROVER_assert(ret == AWS_OP_SUCCESS,
                         "aws_array_list_get_at should succeed when index < length");
        /* Verify that val now contains the expected element */
        uint8_t *expected = (uint8_t *)list_before.data + index * item_size;
        __CPROVER_assert(__builtin_memcmp(val, expected, item_size) == 0,
                         "val must contain a copy of the element at index");
    } else {
        __CPROVER_assert(ret == AWS_ERROR_INVALID_INDEX,
                         "aws_array_list_get_at should return INVALID_INDEX when index >= length");
        /* In the error case, val must remain unchanged */
        __CPROVER_assert(__builtin_memcmp(val, val_before, item_size) == 0,
                         "val must be unchanged on error");
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

    /* Post‑condition: list data contents unchanged */
    __CPROVER_assert(__builtin_memcmp(list.data, data_before, allocation_size) == 0,
                     "list data buffer must be unchanged");

    /* Clean up */
    free(list.data);
    free(data_before);
    free(val_before);
    free(val);
}
