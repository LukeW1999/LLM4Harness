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
    /* val must be readable for list->item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

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
         * - The data at index*item_size should match val (memcpy happened)
         * - If index >= old length, length becomes index + 1
         * - current_size must be >= length * item_size
         */
        assert(list.current_size >= list.length * list.item_size);

        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* The data pointer must be valid (non-null since we wrote to it) */
        assert(list.data != NULL);

        /* Verify the value was actually written */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

    } else {
        /* On failure:
         * - The list should remain valid
         * - length should not have increased beyond what it was
         * - current_size unchanged (capacity didn't grow)
         */
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }
}
