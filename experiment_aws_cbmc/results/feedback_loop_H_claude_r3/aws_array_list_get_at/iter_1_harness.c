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
    const void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The element at index is set to val (memcpy'd)
         * - If index >= old length, length becomes index + 1
         * - item_size unchanged
         * - alloc unchanged
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Length: if index >= old length, new length = index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            /* index < old length, length unchanged */
            assert(list.length == old_list.length);
        }

        /* current_size: may have grown (ensure_capacity), but >= old */
        assert(list.current_size >= old_list.current_size);

        /* The data at the index should match val */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), (const uint8_t *)val, list.item_size);

    } else {
        /* On failure:
         * - ensure_capacity failed (dynamic mode OOM or static mode out of bounds)
         * - list should still be valid
         * - item_size and alloc unchanged
         */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        /* length should not have changed on failure */
        assert(list.length == old_list.length);
    }
}
