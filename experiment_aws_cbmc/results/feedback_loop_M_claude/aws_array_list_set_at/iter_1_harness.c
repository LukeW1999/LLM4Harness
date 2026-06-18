#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a val buffer of item_size bytes */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable for item_size bytes (nondet content is fine) */

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* The data pointer must be non-null (FATAL_PRECONDITION in impl) */
        assert(list.data != NULL);

        /* current_size must be >= (index+1)*item_size after ensure_capacity */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* Length must be at least index+1 */
        assert(list.length >= index + 1);

        /* If index was already within old length, length should be unchanged */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            /* length was updated to index + 1 */
            assert(list.length == index + 1);
        }

        /* The value was copied into the list at the correct position */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure from ensure_capacity or overflow, length and current_size
         * may or may not have changed depending on where the failure occurred.
         * The key invariant is that the list remains valid. */
    }
}
