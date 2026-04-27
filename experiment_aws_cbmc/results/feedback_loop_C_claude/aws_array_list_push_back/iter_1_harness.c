// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_back returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to old_length + 1
//   - list->data: MAY CHANGE (if reallocation occurred due to dynamic growth)
//   - list->current_size: MAY CHANGE (if reallocation occurred)
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - The element at index old_length is set to *val
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_back returns AWS_OP_ERR:
//   - list->length: UNCHANGED
//   - list->data: UNCHANGED (no modification on failure)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - length: CHANGED on success (incremented by 1), UNCHANGED on failure
//     - data: MAY CHANGE on success (realloc), UNCHANGED on failure
//     - current_size: MAY CHANGE on success (realloc), UNCHANGED on failure
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//   val (const void *):
//     - pointed-to memory: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
//   - val points to readable memory of list->item_size bytes: YES (precondition)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 (already guaranteed by is_valid, but be explicit) */
    __CPROVER_assume(list.item_size > 0);

    /* Create a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* === POST-CONDITION ASSERTIONS === */

    /* item_size and alloc are always unchanged */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, length must have increased by 1 */
        assert(list.length == old_length + 1);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* current_size must be >= length * item_size */
        size_t required_size;
        int overflow = aws_mul_size_checked(list.length, list.item_size, &required_size);
        assert(!overflow);
        assert(list.current_size >= required_size);

    } else {
        /* On failure, length must be unchanged */
        assert(list.length == old_length);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }

    /* The list must always be valid after the call */
    assert(aws_array_list_is_valid(&list));
}
