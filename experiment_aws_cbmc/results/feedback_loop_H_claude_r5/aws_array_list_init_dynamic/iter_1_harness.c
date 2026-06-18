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

    /* 3. Call the function under test */
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

        size_t expected_size = initial_item_allocation * item_size;
        assert(list.current_size == expected_size);

        if (list.current_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));

    } else {
        /* On failure:
         * The function returns AWS_OP_ERR.
         * The list was zeroed at the start (AWS_ZERO_STRUCT), so on error
         * the list fields should be zeroed (no partial initialization persists
         * since data is not set on error path, and item_size/alloc are only
         * set after successful allocation).
         * Actually, looking at the implementation: on error, we goto error
         * before setting item_size and alloc, so the list remains zeroed.
         */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
    }
}
