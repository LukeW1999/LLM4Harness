#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* 2. Bound the state space */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* 3. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: item_size and alloc are set */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);

        /* length starts at 0 */
        assert(list.length == 0);

        /* If allocation_size > 0, data must be non-NULL and current_size == allocation_size */
        if (initial_item_allocation > 0) {
            size_t allocation_size = initial_item_allocation * item_size;
            assert(list.current_size == allocation_size);
            assert(list.data != NULL);
        } else {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }

        /* Fatal postcondition from implementation */
        assert(list.current_size == 0 || list.data != NULL);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: list is zeroed out */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
    }
}
