/*  
 * Contract for aws_array_list_init_dynamic  
 * ------------------------------------------------------------  
 * Preconditions:  
 *   - __CPROVER_assume(list != NULL);                // caller provides a valid pointer  
 *   - __CPROVER_assume(alloc != NULL);               // allocator must be non‑null (aws_default_allocator())  
 *   - __CPROVER_assume(item_size > 0);               // size of each element must be > 0  
 *   - __CPROVER_assume(initial_item_allocation is any size_t value);  
 *   - No other assumptions about memory contents.  
 *   
 * Postconditions (validity):  
 *   - list->item_size == item_size  
 *   - list->alloc == alloc  
 *   - list->length == 0 (list is zero‑initialized)  
 *   - If the multiplication overflow check (aws_mul_size_checked) reports overflow, the function returns AWS_OP_ERR, list->current_size == 0 and list->data == NULL.  
 *   - If multiplication succeeds and the computed allocation size is > 0:  
 *        * On success (return == AWS_OP_SUCCESS) → list->current_size == allocation_size, list->data != NULL.  
 *        * On failure (return == AWS_OP_ERR, e.g., allocation failure) → list->current_size == 0, list->data == NULL.  
 *   - If the computed allocation size is 0: list->current_size == 0, list->data == NULL, and the function returns AWS_OP_SUCCESS.  
 *   
 * Frame conditions:  
 *   - No memory outside the aws_array_list structure pointed to by `list` is modified.  
 *   - The allocator state is not examined or modified by this harness.  
 */  

#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation;
    size_t item_size;

    /* nondeterministic inputs */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= SIZE_MAX); /* any size_t value */

    /* Call the function under verification */
    int ret = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Postcondition checks */
    assert(list.item_size == item_size);
    assert(list.alloc == alloc);
    assert(list.length == 0);

    size_t allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);

    if (overflow) {
        /* Overflow case: function must have returned error and left list empty */
        assert(ret == AWS_OP_ERR);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    } else {
        if (allocation_size == 0) {
            /* Zero allocation size: success, but no memory allocated */
            assert(ret == AWS_OP_SUCCESS);
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            if (ret == AWS_OP_SUCCESS) {
                /* Successful allocation */
                assert(list.current_size == allocation_size);
                assert(list.data != NULL);
            } else {
                /* Allocation failed */
                assert(ret == AWS_OP_ERR);
                assert(list.current_size == 0);
                assert(list.data == NULL);
            }
        }
    }

    return 0;
}
