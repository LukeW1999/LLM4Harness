// === STEP 1: SUCCESS PATH ===
// When aws_array_list_front returns AWS_OP_SUCCESS:
//   - val: CHANGES to contain a copy of the first element (list->data[0..item_size-1])
//   - list->data: UNCHANGED (no modification to list)
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_front returns AWS_OP_ERR (list is empty):
//   - val: UNCHANGED
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED (== 0)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - data: UNCHANGED always
//     - length: UNCHANGED always
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//   val (output buffer):
//     - contents: CHANGED on success (copy of first element), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_front_harness(void) {
    /* Declare and initialize the array list with nondeterministic but bounded values */
    struct aws_array_list list;

    /* Bound the list for CBMC tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old val contents */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* Call the function under test */
    int result = aws_array_list_front(&list, val);

    /* === STEP 4: Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === STEP 3: Frame conditions - list fields are always unchanged === */
    assert(list.data == old_list.data);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: Success path === */
        /* list must have had length > 0 */
        assert(old_list.length > 0);

        /* val must now contain a copy of the first element */
        assert(memcmp(val, list.data, list.item_size) == 0);
    } else {
        /* === STEP 2: Failure path === */
        /* list must have been empty */
        assert(old_list.length == 0);

        /* val must be unchanged */
        assert(memcmp(val, old_val, list.item_size) == 0);

        /* result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
