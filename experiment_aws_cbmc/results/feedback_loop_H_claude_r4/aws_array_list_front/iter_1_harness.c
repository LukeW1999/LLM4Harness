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

    /* 3. Allocate val with item_size bytes */
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

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The value was copied into the array at index
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (dynamic mode)
         * - data pointer may have changed (realloc in dynamic mode)
         */

        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* current_size must be >= length * item_size */
        size_t required_size;
        /* We can't call aws_mul_size_checked here easily, but validity covers this */

        /* The data pointer must be non-null since we wrote something */
        assert(list.data != NULL);

        /* current_size must be >= old current_size (capacity never shrinks) */
        assert(list.current_size >= old_list.current_size);

    } else {
        /* On failure:
         * - The list should remain valid (already asserted above)
         * - In static mode: list is unchanged
         * - In dynamic mode: allocation may have failed, list unchanged
         */

        /* Length should not have changed on failure */
        assert(list.length == old_list.length);

        /* current_size should not have changed on failure */
        assert(list.current_size == old_list.current_size);

        /* data pointer should not have changed on failure */
        assert(list.data == old_list.data);
    }
}
