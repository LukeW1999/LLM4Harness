// === STEP 1: SUCCESS PATH ===
// When aws_array_list_back returns AWS_OP_SUCCESS (0):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED (pointer itself unchanged, contents unchanged)
//   - val: CHANGED - contains a copy of the last element (item_size bytes copied from list->data)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_back returns AWS_OP_ERR (list is empty):
//   - list->alloc: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED (== 0)
//   - list->item_size: UNCHANGED
//   - list->data: UNCHANGED
//   - val: UNCHANGED (no memcpy performed)
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - current_size: UNCHANGED always
//     - length: UNCHANGED always
//     - item_size: UNCHANGED always
//     - data: UNCHANGED always (pointer value)
//   val (void*):
//     - contents: CHANGED on success (last element copied in), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (must hold after call, postcondition)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_back_harness(void) {
    /* Allocate and initialize the list with bounded sizes */
    struct aws_array_list list;

    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 (part of validity) */
    __CPROVER_assume(list.item_size > 0);

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Allocate val with enough space for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old val contents */
    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, list.item_size);

    /* Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* === Postcondition: list validity must hold === */
    assert(aws_array_list_is_valid(&list));

    /* === Frame conditions: list fields must be unchanged === */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: list was non-empty */
        assert(old_list.length > 0);

        /* val should contain the last element */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)list.data + last_item_offset,
            list.item_size
        );
    } else {
        /* Failure path: list was empty */
        assert(result == AWS_OP_ERR);
        assert(old_list.length == 0);

        /* val should be unchanged */
        assert_bytes_match((const uint8_t *)val, old_val, list.item_size);
    }
}
