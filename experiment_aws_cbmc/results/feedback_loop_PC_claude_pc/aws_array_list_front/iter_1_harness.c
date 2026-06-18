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

    /* 3. Allocate val with item_size bytes readable */
    /* item_size is bounded by MAX_ITEM_SIZE */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must not change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The memory at index*item_size in list->data contains val
         * - If index >= old length, length becomes index + 1
         * - current_size >= new length * item_size
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            assert(list.length == index + 1);
        }

        /* current_size must accommodate the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* The value was copied into the list at the correct position */
        assert(AWS_MEM_IS_READABLE(list.data, list.item_size * list.length));

        /* Verify the bytes at index match val */
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val, list.item_size);

    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - In static mode (alloc == NULL), capacity cannot grow
         * - The list state should be consistent
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* On failure, length should not have increased beyond what it was */
        /* (ensure_capacity failed, so no write happened) */
        assert(list.length == old_list.length);

        /* current_size should not have changed on failure */
        assert(list.current_size == old_list.current_size);

        /* data pointer should not have changed on failure for static lists */
        if (list.alloc == NULL) {
            assert(list.data == old_list.data);
        }
    }
}
