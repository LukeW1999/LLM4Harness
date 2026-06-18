#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* 2. Bound the state space to keep CBMC tractable */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(allocator != NULL);

    /* 3. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);

    /* 4. Assert postconditions */

    if (result == AWS_OP_SUCCESS) {
        assert(list.item_size == item_size);
        assert(list.alloc == allocator);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);

        if (list.current_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }

        assert(aws_array_list_is_valid(&list));

    } else {
        assert(result == AWS_OP_ERR);
    }
}
