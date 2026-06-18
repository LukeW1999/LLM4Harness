#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - The value was copied into the array at index
         * - If index >= old length, length becomes index + 1
         * - current_size may have grown (dynamic mode)
         */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* current_size must be >= length * item_size */
        size_t required_size = 0;
        int no_overflow = !aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(no_overflow);
        assert(list.current_size >= required_size);
        /* data must be non-null since we wrote something */
        assert(list.data != NULL);
        /* The value at index matches val */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));
    } else {
        /* On failure:
         * - In static mode with out-of-bounds index, list is unchanged
         * - length, current_size, data, alloc, item_size unchanged
         */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.data == old_list.data);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}
