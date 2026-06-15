#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void harness() {
    /* Data structure */
    struct aws_array_list list;
    struct aws_allocator *allocator = nondet_bool() ? NULL : can_fail_allocator(); // but must be non-null per precondition
    // We'll assume allocator is non-null; we can just pass a nondet pointer with assume non-null
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(alloc != NULL);

    /* Non-deterministic inputs with bounds */
    size_t initial_item_allocation;
    size_t item_size;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Preconditions from implementation */
    __CPROVER_assume(item_size > 0);

    /* Call the function */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        if (initial_item_allocation > 0) {
            /* Since multiplication succeeded, allocation_size > 0, so data must be non-null */
            assert(list.data != NULL);
            /* current_size should be the product */
            size_t expected_size;
            if (aws_mul_size_checked(initial_item_allocation, item_size, &expected_size) == AWS_OP_SUCCESS) {
                assert(list.current_size == expected_size);
            }
        } else {
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure, struct is zeroed (since we zeroed and then error before setting fields) */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }
}
