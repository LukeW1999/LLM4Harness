#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_set_at:
 *
 * Doxygen says:
 *   "Copies the memory pointed to by val into the array at index.
 *    If in dynamic mode, the size will grow by a factor of two when the array is full.
 *    In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds."
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - list->data contents at index*item_size are overwritten with val
 *    - list->length may increase (if index >= old length, length = index + 1)
 *    - list->current_size may increase (if dynamic and capacity was insufficient)
 *    - list->data pointer may change (if dynamic realloc occurred)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid
 *    - list->item_size unchanged
 *    - list->alloc unchanged
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) holds after the call
 */

void aws_array_list_set_at_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up the index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Set up val — must be readable for item_size bytes */
    /* item_size is bounded by MAX_ITEM_SIZE from the bounding predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val is readable (malloc guarantees writable/readable memory) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* On success: current_size must be sufficient for length items */
        assert(list.current_size >= list.length * list.item_size);

        /* On success: data must be non-null (we wrote something) */
        assert(list.data != NULL);

        /* If index was already within old length, length should not decrease */
        if (index < old_list.length) {
            assert(list.length >= old_list.length);
        }

        /* If index was exactly at old length, length increases by 1 */
        if (index == old_list.length) {
            assert(list.length == old_list.length + 1);
        }

        /* If index was beyond old length, length becomes index + 1 */
        if (index > old_list.length) {
            assert(list.length == index + 1);
        }
    } else {
        /* On failure: list remains valid (already asserted above) */
        /* length should not have changed on failure */
        assert(list.length == old_list.length);

        /* current_size should not have changed on failure */
        assert(list.current_size == old_list.current_size);
    }
}
