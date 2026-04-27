// === STEP 1: SUCCESS PATH ===
// When aws_array_list_get_at returns AWS_OP_SUCCESS:
//   - val: CHANGES to contain the item at index (memcpy'd from list->data + item_size * index)
//   - list->data: UNCHANGED (no modification to list data)
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   Condition: list->length > index
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_get_at returns AWS_OP_ERR:
//   - val: UNCHANGED (no memcpy performed)
//   - list->data: UNCHANGED
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   Condition: list->length <= index
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - data: UNCHANGED always (get_at only reads, never writes list)
//     - length: UNCHANGED always
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//   val (void*):
//     - contents: CHANGED on success (memcpy from list data), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)
//   - val must point to writable memory of list->item_size bytes

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_get_at_harness() {
    /* Declare and initialize the list */
    struct aws_array_list list;

    /* Bound the list for CBMC tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure list has allocated data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondeterministic index */
    size_t index;
    index = nondet_size_t();

    /* Allocate val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Save old val contents */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* === FRAME CONDITIONS: list fields are always unchanged === */
    assert(list.data == old_list.data);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    /* === VALIDITY INVARIANT === */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* === STEP 1: SUCCESS PATH === */
        /* list->length > index must hold */
        assert(list.length > index);

        /* val must now contain the item at index */
        assert(memcmp(val, (uint8_t *)list.data + (list.item_size * index), list.item_size) == 0);
    } else {
        /* === STEP 2: FAILURE PATH === */
        /* list->length <= index must hold */
        assert(list.length <= index);

        /* val must be unchanged */
        assert(memcmp(val, old_val, list.item_size) == 0);

        /* result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
