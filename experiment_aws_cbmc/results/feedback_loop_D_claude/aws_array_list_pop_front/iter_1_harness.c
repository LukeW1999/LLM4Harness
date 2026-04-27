// === STEP 1: SUCCESS PATH ===
// When aws_array_list_pop_front returns AWS_OP_SUCCESS:
//   - list->length: CHANGES (decremented by 1, via pop_front_n(list, 1))
//   - list->data: may shift contents (memmove), but pointer itself may be UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_pop_front returns AWS_OP_ERR (list is empty):
//   - list->length: UNCHANGED (was 0)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//   - list->data: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: CHANGED on success (decremented by 1), UNCHANGED on failure
//   - item_size: UNCHANGED always
//   - data: UNCHANGED (pointer), contents may shift on success
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    /* Allocate and initialize the array list with nondeterministic but bounded values */
    struct aws_array_list list;

    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is properly allocated */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* === VALIDITY INVARIANT === */
    assert(aws_array_list_is_valid(&list));

    /* === FRAME CONDITIONS: always unchanged === */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH === */
        /* List was non-empty, length decremented by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
    } else {
        /* === FAILURE PATH === */
        /* List was empty */
        assert(result == AWS_OP_ERR);
        assert(old_length == 0);
        assert(list.length == 0);
    }
}
