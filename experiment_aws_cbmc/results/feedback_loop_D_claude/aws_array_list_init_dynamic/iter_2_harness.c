#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 64
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 16
#endif

void aws_array_list_init_dynamic_harness() {
    /* Use stack-allocated list */
    struct aws_array_list list;

    /* Set up a valid allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondeterministic inputs, bounded to keep verification tractable */
    size_t initial_item_allocation;
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */
        
        /* list->alloc must be set to alloc */
        assert(list.alloc == alloc);
        
        /* list->item_size must be set to item_size */
        assert(list.item_size == item_size);
        
        /* list->length must be 0 */
        assert(list.length == 0);
        
        /* Postcondition: current_size == 0 || data != NULL */
        assert(list.current_size == 0 || list.data != NULL);
        
        /* If initial_item_allocation > 0, then current_size > 0 and data != NULL */
        if (initial_item_allocation > 0) {
            assert(list.current_size > 0);
            assert(list.data != NULL);
            assert(list.current_size == initial_item_allocation * item_size);
        } else {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }
        
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
        
    } else {
        /* === FAILURE PATH assertions === */
        assert(result == AWS_OP_ERR);
        
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
    }
}
