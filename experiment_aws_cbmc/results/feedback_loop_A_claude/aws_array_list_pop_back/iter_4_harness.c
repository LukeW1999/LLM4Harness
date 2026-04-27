#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 8
#endif

void aws_array_list_pop_back_harness() {
    /* 1. Declare and set up the array list with nondet values */
    struct aws_array_list list;

    /* Use nondet values for the fields */
    list.alloc = NULL; /* allocator can be NULL for static lists */
    
    /* item_size must be > 0 */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    
    /* current_size must be a multiple of item_size */
    size_t num_items;
    __CPROVER_assume(num_items <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = num_items * list.item_size;
    
    /* length must be <= num_items */
    __CPROVER_assume(list.length <= num_items);
    
    /* Allocate data if current_size > 0 */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Ensure the list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a buffer to receive the popped element */
    void *val = malloc(list.item_size);
    /* val may be NULL - that's fine, pop_back should handle it */

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    /* List must always be valid after the call */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* On success, length must have decreased by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
        /* current_size should not change */
        assert(list.current_size == old_current_size);
    } else {
        /* On failure, length should not have changed */
        assert(list.length == old_length);
        /* On failure, the list was empty */
        assert(old_length == 0);
    }
}
