#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Preconditions from AWS_FATAL_PRECONDITION */
    __CPROVER_assume(item_size > 0);
    /* Bound inputs to keep state space manageable */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /*
         * Success path: no overflow in multiplication and allocation succeeded.
         * Compute expected allocation size safely by assuming no overflow.
         */
        __CPROVER_assume(initial_item_allocation <= SIZE_MAX / item_size);
        size_t expected_allocation_size = initial_item_allocation * item_size;

        assert(list.current_size == expected_allocation_size);
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);

        if (expected_allocation_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }

        /* The initialized list must satisfy the validity predicate */
        assert(aws_array_list_is_valid(&list));
    } else {
        /*
         * Failure path: either overflow or allocation failure.
         * The implementation zeroes the struct before attempting allocation,
         * so on failure all fields remain zero.
         */
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.data == NULL);
        assert(list.alloc == NULL);
        assert(result == AWS_OP_ERR);
    }
}
