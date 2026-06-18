#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic val — must be readable for item_size bytes */
    /* Allocate a buffer for val */
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
         */
        /* Length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was within old bounds, length stays the same or grows */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            assert(list.length == index + 1);
        }

        /* current_size must be >= length * item_size */
        size_t required_size;
        /* We can't call aws_mul_size_checked easily, but validity covers this */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index must match val */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

    } else {
        /* On failure:
         * - In static mode: AWS_ERROR_INVALID_INDEX if index out of bounds
         * - In dynamic mode: allocation failure
         * - List should remain valid (already asserted above)
         * - Length should not have increased beyond what it was
         */
        /* On failure, length should be unchanged */
        assert(list.length == old_list.length);

        /* current_size should be unchanged on failure */
        assert(list.current_size == old_list.current_size);

        /* data pointer should be unchanged on failure */
        assert(list.data == old_list.data);
    }
}
