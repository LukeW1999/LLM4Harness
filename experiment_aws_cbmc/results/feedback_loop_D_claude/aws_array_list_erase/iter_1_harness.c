// === STEP 1: SUCCESS PATH ===
// When aws_array_list_erase returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to (old_length - 1) because pop_front or pop_back is called
//   - list->data: may be modified (memmove for middle element case)
//   - list->current_size: UNCHANGED (no reallocation happens)
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_erase returns AWS_OP_ERR (index >= length):
//   - list->length: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always (no realloc in erase)
//   - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//   - item_size: UNCHANGED always
//   - data: CHANGED on success (content may shift), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_erase_harness() {
    /* Declare and initialize the array list */
    struct aws_array_list list;

    /* Bound the list to make verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 for meaningful operations */
    __CPROVER_assume(list.item_size > 0);

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* === VALIDITY INVARIANT === */
    assert(aws_array_list_is_valid(&list));

    /* === FRAME CONDITIONS: always unchanged === */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: index was valid (index < old_length) */
        assert(index < old_length);
        /* length decremented by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* Failure path: index was out of bounds */
        assert(result == AWS_OP_ERR);
        assert(index >= old_length);
        /* All fields unchanged */
        assert(list.length == old_length);
        assert(list.data == old_list.data);
    }
}
