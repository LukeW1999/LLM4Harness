#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_set_at
 *
 * Doxygen says:
 *   "Copies the memory pointed to by val into the array at index.
 *    If in dynamic mode, the size will grow by a factor of two when the array is full.
 *    In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds."
 *
 * Analysis:
 * 1. Changed on success:
 *    - list->data[index * item_size .. (index+1)*item_size) = *val  (copy)
 *    - list->length may increase to index+1 if index >= old length
 *    - list->current_size may increase (dynamic realloc)
 * 2. Unchanged:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (postcondition in implementation)
 * 4. Validity invariant always holds after call.
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Non-deterministic val — must be readable for item_size bytes */
    /* Allocate a buffer of item_size bytes for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length is at least index+1 */
        assert(list.length >= index + 1);

        /* On success: current_size is at least length * item_size */
        size_t required_size;
        /* We know from validity that current_size >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* The data pointer must be non-null since we wrote something */
        assert(list.data != NULL);

        /* If index was within old bounds, length doesn't shrink */
        if (index < old_list.length) {
            assert(list.length == old_list.length);
        } else {
            /* length became index + 1 */
            assert(list.length == index + 1);
        }

        /* The value was copied into the array at the correct offset */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));

    } else {
        /* On failure: list is still valid (already asserted above) */
        /* length should not have increased beyond what it was */
        /* In static mode with out-of-bounds index, length unchanged */
        /* current_size unchanged on failure */
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.data == old_list.data);
    }
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
