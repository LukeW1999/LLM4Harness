#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and set up the list */
    struct aws_array_list list;

    /* 2. Set up allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Bound scalar inputs */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* 4. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - list.item_size == item_size
         * - list.alloc == alloc
         * - list.length == 0 (AWS_ZERO_STRUCT zeroes everything, length not changed after)
         * - list.current_size == initial_item_allocation * item_size
         * - if current_size > 0, list.data != NULL
         * - if current_size == 0, list.data == NULL
         */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);

        size_t expected_size = initial_item_allocation * item_size;
        assert(list.current_size == expected_size);

        if (list.current_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }

        /* Postcondition from implementation */
        assert(list.current_size == 0 || list.data != NULL);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: AWS_OP_ERR returned
         * The function returns AWS_OP_ERR if:
         * - multiplication overflows
         * - memory allocation fails
         * In both cases, the function returns early after AWS_ZERO_STRUCT or before setting fields
         * We just check the result is AWS_OP_ERR
         */
        assert(result == AWS_OP_ERR);
    }
}
