#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* 2. Bound the state space to keep CBMC tractable */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(alloc != NULL);

    /* 3. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions */

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - list.item_size == item_size
         * - list.alloc == alloc
         * - list.length == 0 (freshly initialized)
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
        /* On failure (AWS_OP_ERR):
         * The function returns error if multiplication overflows or allocation fails.
         * The list is zeroed at the start (AWS_ZERO_STRUCT), so on error path
         * the list fields should be in a zeroed/reset state.
         * The function does not guarantee a valid list on failure,
         * but we can check that the error path was taken correctly.
         */
        assert(result == AWS_OP_ERR);
    }
}

void aws_array_list_init_dynamic_harness(void) {
    aws_array_list_init_dynamic_harness();
    return 0;
}
