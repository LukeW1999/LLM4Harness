#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* 2. Bound the state space to keep CBMC tractable */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions */

    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - item_size is set to the provided item_size
         * - alloc is set to the provided allocator
         * - length is 0 (AWS_ZERO_STRUCT zeroes everything, length not modified)
         * - current_size == initial_item_allocation * item_size (or 0 if allocation is 0)
         * - if current_size > 0, data must be non-NULL
         * - if current_size == 0, data may be NULL
         */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);

        /* current_size should equal initial_item_allocation * item_size */
        size_t expected_size = 0;
        int overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_size);
        /* If we got success, the multiplication must not have overflowed */
        assert(!overflow);
        assert(list.current_size == expected_size);

        /* Postcondition from implementation: current_size == 0 || data != NULL */
        if (list.current_size > 0) {
            assert(list.data != NULL);
        }

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure (AWS_OP_ERR):
         * The function returns early after error. The list may be partially
         * initialized (AWS_ZERO_STRUCT was called), but we just check the
         * result is the error sentinel.
         */
        assert(result == AWS_OP_ERR);
    }
}
