#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* Allocate list on the heap so CBMC can track it */
    struct aws_array_list *list = malloc(sizeof(struct aws_array_list));
    __CPROVER_assume(list != NULL);

    /* Use the default allocator (required by rules) */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Unconstrained inputs */
    size_t initial_item_allocation;
    size_t item_size;

    /* Enforce fatal preconditions */
    __CPROVER_assume(item_size > 0);

    /* Bound inputs to keep CBMC tractable */
    __CPROVER_assume(initial_item_allocation <= 2);
    __CPROVER_assume(item_size <= 8);

    /* Save inputs for postcondition checks */
    size_t old_initial_item_allocation = initial_item_allocation;
    size_t old_item_size = item_size;

    /* Call the function under test */
    int result = aws_array_list_init_dynamic(list, alloc, initial_item_allocation, item_size);

    /* Postcondition: return value is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* item_size and alloc are set correctly */
        assert(list->item_size == old_item_size);
        assert(list->alloc == alloc);

        /* length is always 0 after init */
        assert(list->length == 0);

        /* Check current_size vs allocation */
        size_t expected_size = 0;
        int overflow = aws_mul_size_checked(old_initial_item_allocation, old_item_size, &expected_size);
        assert(!overflow); /* success implies no overflow */
        assert(list->current_size == expected_size);

        /* Fatal postcondition: if current_size > 0 then data != NULL */
        if (list->current_size > 0) {
            assert(list->data != NULL);
        } else {
            /* When allocation_size == 0, data should remain NULL (zeroed struct) */
            assert(list->data == NULL);
        }

        /* Validity check */
        assert(aws_array_list_is_valid(list));

        /* Capacity matches current_size / item_size */
        assert(aws_array_list_capacity(list) == old_initial_item_allocation || list->current_size == 0);

        /* Length is 0 */
        assert(aws_array_list_length(list) == 0);

        /* Clean up to avoid memory leaks */
        aws_array_list_clean_up(list);
    } else {
        /* On error: either overflow occurred or allocation failed */
        size_t dummy = 0;
        int overflow = aws_mul_size_checked(old_initial_item_allocation, old_item_size, &dummy);
        /* Either there was an overflow, or the allocator failed (nondeterministic) */
        /* We cannot assert much here beyond the return value being AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }

    return 0;
}
