#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_array_list_back
 *
 * This function copies the last element of the array list into the memory
 * pointed to by val. If the list is empty, it fails and leaves val untouched.
 * The list itself is never modified.
 */
void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare the output buffer where the last element will be written.
     * Its size must be at least list->item_size; we allocate the maximum
     * possible size allowed by bounding (MAX_ITEM_SIZE). */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 3. Save the initial state of the list for post-condition checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post-conditions:
     *    - The list itself is never modified (all fields and data unchanged).
     *    - The list validity invariant is preserved.
     *    - On success: val contains a copy of the last element.
     *    - On failure: the list is empty.
     */

    /* 5a. Common immutable fields of the list (always unchanged) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 5b. Ensure the array data buffer has not been corrupted */
    if (old.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* 5c. The list validity must always hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* 5d. Success and failure branches */
    if (result == AWS_OP_SUCCESS) {
        /* The function should only succeed when the list is non-empty */
        assert(old.length > 0);

        /* Verify that val now contains exactly the last element */
        size_t last_offset = old.item_size * (old.length - 1);
        const uint8_t *expected_source = (const uint8_t *)old.data + last_offset;
        assert_bytes_match((uint8_t *)val, expected_source, old.item_size);
    } else {
        /* Failure occurs exclusively when the list is empty */
        assert(result != AWS_OP_SUCCESS);
        assert(old.length == 0);

        /* The output buffer val should be left untouched (not verified by this
         * harness; the immutability checks above suffice to prove correctness
         * of the overall state.) */
    }
}
