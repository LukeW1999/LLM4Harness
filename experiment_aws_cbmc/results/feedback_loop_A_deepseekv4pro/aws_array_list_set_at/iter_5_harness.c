#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Bounds to keep the state space tractable
#define MAX_ITEM_SIZE 4
#define MAX_LENGTH   2

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    // Valid list with default allocator
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Bound the list parameters
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length > 0 && list.length <= MAX_LENGTH);
    __CPROVER_assume(list.current_size >= list.length * list.item_size);
    __CPROVER_assume(list.current_size <= MAX_LENGTH * MAX_ITEM_SIZE);

    // Ensure the data buffer is present and writable
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(__CPROVER_w_ok(list.data, list.current_size));

    // Index must be in bounds (success precondition)
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    // Allocate and initialize the value to set
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, list.item_size));

    // Prevent overlapping memory between source and destination
    __CPROVER_assume((char *)val + list.item_size <= (char *)list.data ||
                     (char *)list.data + list.current_size <= (char *)val);

    // Save a copy of the old list for postcondition comparison
    struct aws_array_list old_list = list;

    // Save the entire old data buffer content for bytewise comparison
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = (uint8_t *)malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_assume(__CPROVER_r_ok(old_data, list.current_size));
        __CPROVER_assume(__CPROVER_w_ok(old_data, list.current_size));
        for (size_t i = 0; i < list.current_size; i++) {
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    int result = aws_array_list_set_at(&list, val, index);

    // Postconditions for a successful call
    assert(result == AWS_OP_SUCCESS);
    assert(aws_array_list_is_valid(&list));

    // List dimensions unchanged
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    // The element at index now matches val
    assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                       val, list.item_size);

    // All bytes outside the modified element remain unchanged
    if (old_data != NULL) {
        size_t modified_start = index * list.item_size;
        size_t modified_end = modified_start + list.item_size;
        for (size_t i = 0; i < old_list.current_size; i++) {
            if (i >= modified_start && i < modified_end) continue;
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    free(val);
    if (old_data != NULL) free(old_data);
}
