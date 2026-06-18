#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    /* Nondeterministic item size within bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    list.item_size = item_size;

    /* Bounded capacity */
    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = current_size;

    /* Length must not exceed capacity */
    size_t length;
    __CPROVER_assume(length <= current_size);
    list.length = length;

    /* Allocate data; a zero capacity may have a NULL data pointer */
    if (current_size > 0) {
        list.data = malloc(current_size * item_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Use the standard non-failing allocator */
    list.alloc = aws_default_allocator();

    /* Ensure the constructed list satisfies all invariants */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Value to be pushed – nondet content */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;

    /* Save the old state */
    struct aws_array_list old = list;

    /* Save a representative byte from the old data buffer (if any) */
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data,
                             list.current_size * list.item_size,
                             &old_byte);
    } else {
        old_byte.byte = 0;
    }

    /* Call the function under verification */
    int rval = aws_array_list_push_front(&list, val);

    /* Postconditions */
    if (rval == AWS_OP_SUCCESS) {
        /* Success: length increased by one */
        assert(list.length == old.length + 1);

        /* item_size and allocator are unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Capacity never shrinks */
        assert(list.current_size >= old.current_size);

        /* The list stays valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: the list state is completely unmodified */
        assert_array_list_equivalence(&list, &old, &old_byte);
        assert(aws_array_list_is_valid(&list));
    }

    /* Free the allocated data to avoid memory leak and satisfy free preconditions */
    if (list.data != NULL) {
        free(list.data);
    }
}
