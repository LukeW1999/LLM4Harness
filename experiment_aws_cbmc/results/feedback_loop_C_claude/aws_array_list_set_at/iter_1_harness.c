// === STEP 1: SUCCESS PATH ===
// When aws_array_list_set_at returns AWS_OP_SUCCESS:
//   - list->data: CHANGES (memcpy writes val into list->data at offset index*item_size)
//   - list->length: CHANGES to index+1 if index >= old length, otherwise UNCHANGED
//   - list->current_size: MAY CHANGE (if ensure_capacity reallocates)
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_set_at returns AWS_OP_ERR:
//   - list->data: UNCHANGED (or possibly changed by ensure_capacity before failure)
//   - list->length: UNCHANGED
//   - list->current_size: MAY CHANGE (ensure_capacity may have partially succeeded)
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - item_size: UNCHANGED always
//     - length: CHANGED on success if index >= old_length, UNCHANGED otherwise or on failure
//     - current_size: MAY CHANGE (ensure_capacity)
//     - data: CHANGED on success (memcpy), possibly changed by ensure_capacity
//   val (const void *):
//     - pointed-to memory: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after successful call)
//   - On success: list->length >= index + 1
//   - On success: list->data != NULL (AWS_FATAL_PRECONDITION)
//   - list->item_size: UNCHANGED always
//   - list->alloc: UNCHANGED always

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* Setup: create a bounded array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* Setup: create a valid val pointer */
    /* item_size must be > 0 for val to be readable */
    __CPROVER_assume(list.item_size > 0);
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Choose a nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, data must not be NULL */
        assert(list.data != NULL);

        /* On success, length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was >= old_length, length should now be index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }

        /* The data at the index should match val (memcpy was done) */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, val, list.item_size);

    } else {
        /* On failure: length must be unchanged */
        assert(list.length == old_length);

        /* On failure: the list validity should still hold if data is valid */
        /* (ensure_capacity may have failed, so data could be NULL for dynamic lists) */
        /* We only assert validity if data is non-NULL */
        if (list.data != NULL) {
            assert(aws_array_list_is_valid(&list));
        }
    }
}
