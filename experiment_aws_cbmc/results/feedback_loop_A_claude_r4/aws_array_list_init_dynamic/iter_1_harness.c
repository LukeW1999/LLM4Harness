#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* 2. Bound the state space */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - list.item_size == item_size
         * - list.alloc == alloc
         * - list.length == 0 (zeroed struct)
         * - list.current_size == initial_item_allocation * item_size
         * - if current_size > 0, list.data != NULL
         * - if current_size == 0, list.data == NULL
         */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);

        size_t expected_size = 0;
        /* We know multiplication succeeded since the function returned success */
        assert(list.current_size == initial_item_allocation * item_size);

        if (list.current_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: function returns AWS_OP_ERR.
         * The list may be in a partially initialized state (zeroed),
         * but we check that the validity invariant holds for the zeroed state
         * or that data is NULL when current_size is 0.
         */
        /* After AWS_ZERO_STRUCT, all fields are 0/NULL */
        /* The function zeros the struct before attempting allocation */
        /* On error path, list.data may be NULL and current_size may be 0 */
        assert(result == AWS_OP_ERR);
        /* The postcondition from the implementation: current_size == 0 || data != NULL */
        assert(list.current_size == 0 || list.data != NULL);
    }
}
