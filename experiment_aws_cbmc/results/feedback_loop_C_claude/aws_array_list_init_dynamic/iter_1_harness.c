// === STEP 1: SUCCESS PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_SUCCESS:
//   - list->alloc: CHANGES to alloc
//   - list->current_size: CHANGES to initial_item_allocation * item_size (or 0 if allocation is 0)
//   - list->length: CHANGES to 0 (AWS_ZERO_STRUCT zeroes it)
//   - list->item_size: CHANGES to item_size
//   - list->data: CHANGES to allocated memory (or NULL if allocation_size == 0)
//   - Postcondition: list->current_size == 0 || list->data != NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_ERR:
//   - list->alloc: UNCHANGED (zeroed by AWS_ZERO_STRUCT, then not set)
//   - list->current_size: UNCHANGED (zeroed, then not set)
//   - list->length: UNCHANGED (zeroed)
//   - list->item_size: UNCHANGED (zeroed, then not set)
//   - list->data: UNCHANGED (zeroed, then not set or freed)
//   Note: AWS_ZERO_STRUCT is called first, so on failure the list is zeroed out
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: CHANGED on success (set to alloc), zeroed on failure
//     - current_size: CHANGED on success (set to allocation_size), zeroed on failure
//     - length: CHANGED always (zeroed by AWS_ZERO_STRUCT)
//     - item_size: CHANGED on success (set to item_size), zeroed on failure
//     - data: CHANGED on success (allocated or NULL), zeroed on failure
//   alloc (struct aws_allocator *):
//     - UNCHANGED (only used for allocation, not modified)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after successful call)
//   - On success: list.item_size == item_size
//   - On success: list.alloc == alloc
//   - On success: list.length == 0
//   - On success: list.current_size == 0 || list.data != NULL
//   - On failure: list is zeroed (alloc==NULL, current_size==0, length==0, item_size==0, data==NULL)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_init_dynamic_harness(void) {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = can_fail_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* Preconditions: alloc != NULL, item_size > 0 */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);

    /* Bound the inputs to keep verification tractable */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        /* Postcondition from implementation */
        assert(list.current_size == 0 || list.data != NULL);
        /* If allocation_size > 0, current_size should equal initial_item_allocation * item_size */
        /* and data should be non-null */
        if (initial_item_allocation > 0) {
            /* allocation succeeded, so data is non-null and current_size > 0 */
            assert(list.data != NULL);
            assert(list.current_size > 0);
            assert(list.current_size == initial_item_allocation * item_size);
        } else {
            /* No allocation needed */
            assert(list.current_size == 0);
        }
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* === FAILURE PATH assertions === */
        /* AWS_ZERO_STRUCT is called first, so list fields are zeroed */
        /* Either mul overflow or aws_mem_acquire failed */
        assert(result == AWS_OP_ERR);
        /* The list should be in a zeroed state (AWS_ZERO_STRUCT was called) */
        /* Note: on failure, the goto skips setting alloc/item_size/current_size */
        /* so they remain at their zeroed values */
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.data == NULL);
    }
}
