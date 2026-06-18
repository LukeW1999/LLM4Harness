#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a non-deterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a non-deterministic val pointer with readable memory of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must NOT change regardless of result */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - data must be non-null (FATAL_PRECONDITION ensures this)
         * - The element at index must match val
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (ensure_capacity may have reallocated)
         */
        assert(list.data != NULL);

        /* The memory at index should match val */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);

        /* Length update: if index was >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_list.length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

    } else {
        /* On failure:
         * - ensure_capacity failed OR aws_add_size_checked failed
         * - The list should remain valid
         * - item_size and alloc unchanged (already asserted above)
         * - length and current_size should be unchanged (no modification occurred)
         *   Note: ensure_capacity may have partially changed current_size/data on some paths,
         *   but if it returned error, the list should still be valid.
         */
        /* We can assert that length didn't increase beyond what it was
         * (the function returns error before updating length if ensure_capacity fails,
         * or if add_size_checked fails after memcpy) */
        /* The list is still valid */
        assert(aws_array_list_is_valid(&list));
    }
}

void aws_array_list_set_at_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
