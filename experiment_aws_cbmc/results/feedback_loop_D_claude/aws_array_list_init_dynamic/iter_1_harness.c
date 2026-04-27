// === STEP 1: SUCCESS PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_SUCCESS:
//   - list->alloc: CHANGES to alloc parameter
//   - list->item_size: CHANGES to item_size parameter
//   - list->length: CHANGES to 0 (via AWS_ZERO_STRUCT)
//   - list->current_size: CHANGES to initial_item_allocation * item_size (if > 0), else 0
//   - list->data: CHANGES to allocated memory (if allocation_size > 0), else NULL
//   - Postcondition: list->current_size == 0 || list->data != NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_init_dynamic returns AWS_OP_ERR:
//   - list->alloc: ZEROED (AWS_ZERO_STRUCT was called before error)
//   - list->item_size: ZEROED
//   - list->length: ZEROED
//   - list->current_size: ZEROED
//   - list->data: ZEROED (NULL)
//   Failure can happen due to:
//     1. aws_mul_size_checked overflow
//     2. aws_mem_acquire returning NULL
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list *):
//     - alloc: CHANGED (set to alloc on success, 0 on failure due to ZERO_STRUCT)
//     - current_size: CHANGED (set to allocation_size on success, 0 on failure)
//     - length: CHANGED to 0 always (ZERO_STRUCT)
//     - item_size: CHANGED (set to item_size on success, 0 on failure)
//     - data: CHANGED (allocated on success if needed, NULL on failure)
//   alloc (struct aws_allocator *):
//     - unchanged (we don't modify the allocator itself)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(list): YES (must hold after successful call)
//   - On failure: list is zeroed, aws_array_list_is_valid may not hold (item_size=0)

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_init_dynamic_harness() {
    /* Allocate and set up the list */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Set up a valid allocator */
    struct aws_allocator *alloc = nondet_bool() ? aws_default_allocator() : can_fail_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondeterministic inputs, bounded to keep verification tractable */
    size_t initial_item_allocation;
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Call the function under test */
    int result = aws_array_list_init_dynamic(list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */
        
        /* list->alloc must be set to alloc */
        assert(list->alloc == alloc);
        
        /* list->item_size must be set to item_size */
        assert(list->item_size == item_size);
        
        /* list->length must be 0 */
        assert(list->length == 0);
        
        /* Postcondition: current_size == 0 || data != NULL */
        assert(list->current_size == 0 || list->data != NULL);
        
        /* If initial_item_allocation > 0, then current_size > 0 and data != NULL */
        if (initial_item_allocation > 0) {
            /* allocation_size = initial_item_allocation * item_size > 0 */
            assert(list->current_size > 0);
            assert(list->data != NULL);
            assert(list->current_size == initial_item_allocation * item_size);
        } else {
            /* No allocation needed */
            assert(list->current_size == 0);
            assert(list->data == NULL);
        }
        
        /* Validity invariant */
        assert(aws_array_list_is_valid(list));
        
    } else {
        /* === FAILURE PATH assertions === */
        assert(result == AWS_OP_ERR);
        
        /* After AWS_ZERO_STRUCT, all fields are zeroed before error path */
        /* The list was zeroed at the start, and on error we return without setting fields */
        assert(list->length == 0);
        assert(list->current_size == 0);
        assert(list->data == NULL);
        assert(list->alloc == NULL);
        assert(list->item_size == 0);
    }
}
