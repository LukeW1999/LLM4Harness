#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Allocate a val buffer of item_size bytes */
    /* item_size is guaranteed > 0 by aws_array_list_is_valid */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* Save a byte from val to check it wasn't modified */
    /* (val is input, should not be modified by set_at) */

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must NOT change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The element at index is set to val (memcpy'd)
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (ensure_capacity may have reallocated)
         * - current_size >= length * item_size
         */

        /* Length postcondition */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* Capacity must be sufficient for the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we have at least one element */
        assert(list.data != NULL);

        /* The value at index must match val */
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - length should not have increased beyond what it was
         *   (either unchanged or ensure_capacity failed before any write)
         * - item_size and alloc unchanged (already asserted)
         */

        /* On failure, length should be unchanged */
        assert(list.length == old_list.length);

        /* current_size should be unchanged on failure */
        assert(list.current_size == old_list.current_size);

        /* data pointer should be unchanged on failure */
        assert(list.data == old_list.data);
    }

    /* item_size and alloc are always unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
}
