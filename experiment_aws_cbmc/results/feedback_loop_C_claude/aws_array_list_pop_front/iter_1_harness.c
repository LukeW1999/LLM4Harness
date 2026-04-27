// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_front returns AWS_OP_SUCCESS:
//   - list->length: CHANGES to (old_length - 1) because pop_front_n(list, 1) shifts elements
//   - list->data: may be shifted (elements moved), but pointer itself unchanged
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_front returns AWS_OP_ERR (list is empty):
//   - list->length: UNCHANGED (still 0)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//     - current_size: UNCHANGED always
//     - item_size: UNCHANGED always
//     - alloc: UNCHANGED always
//     - data: pointer UNCHANGED always (content may shift on success)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold before and after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_front_harness(void) {
    /* Allocate and initialize the array list with bounded size */
    struct aws_array_list list;

    /* Bound the list for CBMC tractability */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the list has allocated data */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* === Validity invariant === */
    assert(aws_array_list_is_valid(&list));

    /* === Frame conditions: always unchanged === */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);

    if (old_length > 0) {
        /* === SUCCESS PATH === */
        assert(result == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);
    } else {
        /* === FAILURE PATH === */
        assert(result == AWS_OP_ERR);
        assert(list.length == old_length); /* still 0 */
    }
}
