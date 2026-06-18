#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic inputs */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* preconditions for a valid list initialization */
    __CPROVER_assume(item_size > 0);
    /* avoid overflow in multiplication */
    __CPROVER_assume(initial_item_allocation <= SIZE_MAX / item_size);
    /* restrict to avoid allocation failures in this harness */
    __CPROVER_assume(initial_item_allocation == 0);

    /* initialize the list (dynamic mode) */
    __CPROVER_assume(
        aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size) == AWS_OP_SUCCESS);

    /* list must satisfy its own validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of input state */
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    size_t old_length       = list.length;
    void *old_data          = list.data;

    /* call the function under test */
    size_t result = aws_array_list_capacity(&list);

    
}
