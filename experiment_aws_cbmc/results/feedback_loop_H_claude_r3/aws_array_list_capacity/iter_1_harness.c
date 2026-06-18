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

    /* 3. Non-deterministic val pointer with sufficient readable memory */
    /* val must point to readable memory of list->item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The data was copied into the array at index
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (dynamic mode)
         * - data pointer may have changed (realloc in dynamic mode)
         */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* current_size must be at least enough to hold the new length */
        size_t required_size;
        int no_overflow = !aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(no_overflow);
        assert(list.current_size >= required_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - length should not have increased beyond what it was
         *   (ensure_capacity failed, so no write happened)
         * - In static mode, current_size and data are unchanged
         * - In dynamic mode, alloc is non-null; current_size may or may not change
         *   but length should be unchanged
         */
        assert(list.length == old_list.length);
        /* If static (no alloc), current_size and data are unchanged */
        if (old_list.alloc == NULL) {
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}
