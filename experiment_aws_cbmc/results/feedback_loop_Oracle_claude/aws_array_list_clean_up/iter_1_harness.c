#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Nondeterministic initialization */
    size_t item_size;
    size_t initial_item_allocation;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    bool is_dynamic;
    __CPROVER_assume(is_dynamic == 0 || is_dynamic == 1);

    if (is_dynamic) {
        /* Dynamic list */
        if (initial_item_allocation == 0) {
            list.alloc = aws_default_allocator();
            list.data = NULL;
            list.current_size = 0;
            list.length = 0;
            list.item_size = item_size;
        } else {
            list.alloc = aws_default_allocator();
            list.current_size = initial_item_allocation * item_size;
            list.item_size = item_size;
            __CPROVER_assume(list.length <= initial_item_allocation);
            if (list.current_size > 0) {
                list.data = malloc(list.current_size);
                __CPROVER_assume(list.data != NULL);
            } else {
                list.data = NULL;
            }
        }
    } else {
        /* Static list */
        list.alloc = NULL;
        list.item_size = item_size;
        list.current_size = initial_item_allocation * item_size;
        __CPROVER_assume(list.length <= initial_item_allocation);
        if (list.current_size > 0) {
            list.data = malloc(list.current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }
    }

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* Postconditions */

    /* After clean_up, the list should be zeroed out */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
}

void aws_array_list_clean_up_harness(void) {
    aws_array_list_clean_up_harness();
    return 0;
}
