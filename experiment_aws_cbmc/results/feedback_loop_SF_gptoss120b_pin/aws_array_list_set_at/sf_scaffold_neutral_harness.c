#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_is_valid(&list, alloc);

    size_t index = nondet_size_t();

    /* Allocate a buffer for val with the appropriate item size */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* Precondition: val must be readable for list->item_size bytes */
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Precondition: the list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of state before the call */
    size_t old_length = list.length;
    void *old_data = list.data;

    /* Call the function under verification */
    aws_array_list_set_at(&list, val, index);

    

    /* Clean up */
    free(val);
    aws_array_list_clean_up(&list);
}
