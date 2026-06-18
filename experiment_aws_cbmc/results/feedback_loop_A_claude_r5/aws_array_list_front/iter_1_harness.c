#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

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
        /* On success: the element was written at index */
        /* current_size must be >= (index+1) * item_size */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within bounds, length stays the same or grows */
        if (index >= old_list.length) {
            /* length should be index + 1 */
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within existing length */
            assert(list.length == old_list.length);
        }

        /* data pointer must be non-null since we wrote something */
        assert(list.data != NULL);

        /* The value at index matches val */
        assert(AWS_BYTES_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

    } else {
        /* On failure: list should still be valid (already asserted above) */
        /* current_size and length should not have grown beyond what they were */
        /* The list remains valid - no guarantee about unchanged fields beyond validity */
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
    }
}
