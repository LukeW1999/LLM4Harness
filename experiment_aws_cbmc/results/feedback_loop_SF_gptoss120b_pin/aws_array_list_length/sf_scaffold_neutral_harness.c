#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;

    /* Nondeterministic fields */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.length = nondet_size_t();

    list.current_size = nondet_size_t();

    /* Ensure current_size is sufficient for length * item_size */
    {
        size_t required = 0;
        if (aws_mul_size_checked(list.length, list.item_size, &required) == AWS_OP_SUCCESS) {
            __CPROVER_assume(list.current_size >= required);
        } else {
            __CPROVER_assume(0); /* overflow not allowed */
        }
    }

    /* Allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Allocator: dynamic if nondet_bool true, otherwise static */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    /* Assume the list is valid according to its invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of original state */
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    size_t result = aws_array_list_length(&list);

    

    /* Clean up */
    free(list.data);
}
