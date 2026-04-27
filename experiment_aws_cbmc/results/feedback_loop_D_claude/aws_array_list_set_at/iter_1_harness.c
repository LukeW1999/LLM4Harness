// === STEP 1: SUCCESS PATH ===
// When aws_array_list_set_at returns AWS_OP_SUCCESS:
//   - list->data: the bytes at (data + item_size * index) are overwritten with val
//   - list->length: if index >= old length, length = index + 1; otherwise unchanged
//   - list->current_size: may change (via ensure_capacity which may realloc)
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_set_at returns AWS_OP_ERR:
//   - list->data: UNCHANGED (or possibly changed by ensure_capacity partial work, but logically unchanged)
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED (or possibly changed by ensure_capacity)
//   - list->alloc: UNCHANGED
//   - list->item_size: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - item_size: UNCHANGED always
//     - length: CHANGED on success if index >= old_length (set to index+1), else UNCHANGED
//     - current_size: may change on success (ensure_capacity), UNCHANGED on failure
//     - data: bytes at index*item_size changed on success
//   val (const void *):
//     - contents: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after successful call)
//   - val readable: must hold before call (precondition)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* Setup list with bounded parameters */
    struct aws_array_list list;
    
    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Setup val - must be readable for item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: alloc and item_size never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* List must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* data must be non-null after successful set */
        assert(list.data != NULL);

        /* Length update: if index >= old_length, new length = index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* The value at index should match val */
        uint8_t *stored = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(stored, (const uint8_t *)val, list.item_size);

    } else {
        /* On failure, length should be unchanged */
        assert(list.length == old_length);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
    }
}
