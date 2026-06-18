/*  
 * Contract for aws_array_list_init_dynamic  
 * Preconditions:  
 *   - list != NULL  
 *   - alloc != NULL (obtained via aws_default_allocator())  
 *   - item_size > 0  
 *   - initial_item_allocation is any size_t value  
 *   - If multiplication initial_item_allocation * item_size overflows, the function must return AWS_OP_ERR.  
 * Postconditions (validity):  
 *   - On return == AWS_OP_SUCCESS:  
 *       * list->alloc == alloc  
 *       * list->item_size == item_size  
 *       * list->current_size == initial_item_allocation * item_size (or 0 if that product is 0)  
 *       * list->data != NULL iff list->current_size > 0  
 *       * list->length == 0  
 *   - On return == AWS_OP_ERR:  
 *       * All fields of *list are zero (alloc, data, current_size, length, item_size).  
 * Frame:  
 *   - No memory outside the aws_array_list structure (and the optionally allocated data buffer) is modified.  
 */

#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* Allocate the list structure */
    struct aws_array_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondeterministic inputs */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* Compute overflow condition for the multiplication */
    bool overflow = false;
    size_t expected_allocation = 0;
    if (item_size != 0 && initial_item_allocation > SIZE_MAX / item_size) {
        overflow = true;
    } else {
        expected_allocation = initial_item_allocation * item_size;
    }

    /* Call the function under verification */
    int rc = aws_array_list_init_dynamic(list, alloc, initial_item_allocation, item_size);

    /* Postcondition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* Success implies no overflow */
        assert(!overflow);

        /* Basic field invariants */
        assert(list->alloc == alloc);
        assert(list->item_size == item_size);
        assert(list->current_size == expected_allocation);
        assert(list->length == 0);

        if (expected_allocation > 0) {
            assert(list->data != NULL);
        } else {
            assert(list->data == NULL);
        }
    } else {
        /* Failure must be AWS_OP_ERR */
        assert(rc == AWS_OP_ERR);

        /* All fields must remain zeroed */
        assert(list->alloc == NULL);
        assert(list->item_size == 0);
        assert(list->current_size == 0);
        assert(list->length == 0);
        assert(list->data == NULL);
    }

    /* Clean up */
    if (list->data != NULL) {
        aws_mem_release(list->alloc, list->data);
    }
    free(list);
    return 0;
}
