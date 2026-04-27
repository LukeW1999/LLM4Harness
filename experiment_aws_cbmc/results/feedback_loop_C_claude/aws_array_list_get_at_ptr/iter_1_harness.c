// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_SUCCESS:
//   - *val: CHANGES to point to list->data + (list->item_size * index)
//   - list fields: ALL UNCHANGED (read-only operation)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at_ptr returns AWS_OP_ERR:
//   - *val: UNCHANGED (not written)
//   - list fields: ALL UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: UNCHANGED always
// val (void **):
//   - *val: CHANGED on success (set to pointer into list->data), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* Set up a bounded array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Nondeterministic index */
    size_t index;

    /* Nondeterministic output pointer */
    void *val;
    void *old_val = val;

    /* Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* Validity invariant must hold after call */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: list fields must be unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: index < length */
        assert(index < list.length);
        /* *val must point to the correct location in list->data */
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure path: index >= length */
        assert(result == AWS_OP_ERR);
        assert(list.length <= index);
    }
}
