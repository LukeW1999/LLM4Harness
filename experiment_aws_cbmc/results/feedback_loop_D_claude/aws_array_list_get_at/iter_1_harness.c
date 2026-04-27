// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at returns AWS_OP_SUCCESS:
//   - list->data: UNCHANGED (no modification to list)
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - val: CHANGES to contain the bytes from list->data at offset (item_size * index)
//   - Condition: aws_array_list_length(list) > index
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at returns AWS_OP_ERR (raises AWS_ERROR_INVALID_INDEX):
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - val: UNCHANGED (no memcpy performed)
//   - Condition: aws_array_list_length(list) <= index
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - data: UNCHANGED always (get_at is read-only on the list)
//   - length: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - alloc: UNCHANGED always
// val (void*):
//   - contents: CHANGED on success (memcpy from list data), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (must hold after call, postcondition)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_get_at_harness(void) {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Bound the list for tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Nondeterministic index */
    size_t index;
    index = nondet_size_t();

    /* Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old val contents */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* === POSTCONDITIONS === */

    /* Validity invariant: list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: list fields are UNCHANGED always */
    assert(list.data == old_list.data);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: index < length */
        assert(list.length > index);

        /* val should contain the bytes from list->data at offset item_size * index */
        assert(memcmp(val, (uint8_t *)list.data + (list.item_size * index), list.item_size) == 0);
    } else {
        /* Failure path: index >= length */
        assert(result == AWS_OP_ERR);
        assert(list.length <= index);

        /* val should be unchanged */
        assert(memcmp(val, old_val, list.item_size) == 0);
    }
}
