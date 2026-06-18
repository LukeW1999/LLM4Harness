#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Non-deterministic allocation parameters, bounded to keep state space finite */
    size_t initial_item_allocation;
    size_t item_size;

    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 2. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 3. Assert postconditions */

    /* Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: item_size and alloc are set correctly */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);

        /* length starts at 0 */
        assert(list.length == 0);

        /* If initial_item_allocation == 0, current_size == 0 and data == NULL */
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            /* current_size == initial_item_allocation * item_size */
            size_t expected_size = initial_item_allocation * item_size;
            assert(list.current_size == expected_size);
            /* data must be non-NULL when current_size > 0 */
            assert(list.data != NULL);
        }

        /* FATAL_POSTCONDITION: current_size == 0 || data != NULL */
        assert(list.current_size == 0 || list.data != NULL);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));

    } else {
        /* On failure: the function returns AWS_OP_ERR.
         * The list was zero-initialized at the start of the function (AWS_ZERO_STRUCT),
         * and on error we jump to the error label which just returns AWS_OP_ERR.
         * So the list fields should be zeroed (as set by AWS_ZERO_STRUCT before the error). */
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }
}
