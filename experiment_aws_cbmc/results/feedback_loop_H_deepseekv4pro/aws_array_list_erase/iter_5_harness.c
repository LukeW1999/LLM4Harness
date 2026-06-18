#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;

    /* Set up the allocator */
    list.alloc = aws_default_allocator();

    /* Non-deterministic item_size, capacity and length */
    list.item_size = nondet_size_t();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();

    /* Bound and constrain to a valid state */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.length <= list.current_size);

    /* Ensure multiplication does not overflow */
    size_t alloc_size;
    __CPROVER_assume(!__CPROVER_overflow_mult(list.current_size, list.item_size));
    alloc_size = list.current_size * list.item_size;

    /* Give the list a valid, writable data region */
    void *data = malloc(alloc_size);
    __CPROVER_assume(data != NULL);
    list.data = data;

    /* Enforce the full validity invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the old state for later comparisons */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage;
    save_byte_from_array((const uint8_t *)list.data, alloc_size, &old_storage);

    /* Non-deterministic index to erase */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int result = aws_array_list_erase(&list, index);

    /* Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must have been decremented by one */
        assert(aws_array_list_length(&list) == old.length - 1);

        /* Structural fields unchanged */
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Elements located before the removed index are untouched */
        if (index > 0) {
            assert_bytes_match((const uint8_t *)list.data,
                               (const uint8_t *)old.data,
                               index * list.item_size);
        }

        /* The tail after the removed element is shifted left by one slot */
        if (index < old.length - 1) {
            size_t shift_bytes = (old.length - index - 1) * list.item_size;
            assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                               (const uint8_t *)old.data + (index + 1) * list.item_size,
                               shift_bytes);
        }

        /* The list remains valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* The only failure condition is an out-of-bounds index */
        assert(index >= old.length);

        /* The list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_storage);

        /* Length unchanged */
        assert(aws_array_list_length(&list) == old.length);

        /* Validity invariant still holds */
        assert(aws_array_list_is_valid(&list));
    }

    free(data);
}
