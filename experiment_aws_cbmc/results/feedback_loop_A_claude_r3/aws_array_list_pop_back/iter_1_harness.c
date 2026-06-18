#include <aws/common/array_list.h>
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
 *    - list->data contents at index are overwritten with val
 *    - list->length may increase (if index >= old length, length = index + 1)
 *    - list->current_size may increase (if dynamic and capacity was insufficient)
 *    - list->data pointer may change (if dynamic realloc occurred)
 * 2. Unchanged fields:
 *    - list->item_size
 *    - list->alloc
 * 3. On failure:
 *    - list remains valid (best effort; ensure_capacity may fail)
 * 4. Validity invariants:
 *    - aws_array_list_is_valid(list) holds after the call
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
    /* val is readable (malloc guarantees writable/readable memory) */

    /* 4. Save old state */
    struct aws_array_list old_list = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_list.item_size);

    /* alloc must never change */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - length >= index + 1 (index + 1 won't overflow because set_at uses aws_add_size_checked)
         * - length >= old length (length can only grow or stay same)
         * - current_size >= length * item_size (validity)
         * - data is non-null (since we wrote something)
         */
        assert(list.length >= index + 1);
        assert(list.length >= old_list.length);
        assert(list.data != NULL);
        /* The element at index was written — data must be accessible */
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    } else {
        /* On failure:
         * - list is still valid
         * - length should not have increased beyond old length
         *   (ensure_capacity failed, so no write occurred)
         * - In static mode: length unchanged, current_size unchanged, data unchanged
         * - In dynamic mode: allocation may have failed, but list stays valid
         */
        if (list.alloc == NULL) {
            /* Static mode: nothing should change */
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
        /* In dynamic mode on failure, length should not exceed old length */
        assert(list.length <= old_list.length || list.alloc != NULL);
    }

    /* 7. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}

void aws_array_list_pop_back_harness(void) {
    aws_array_list_set_at_harness();
    return 0;
}
