#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness(void) {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* Preconditions: alloc != NULL, item_size > 0 */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);

    /* Bound the inputs to keep verification tractable */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        /* Postcondition from implementation */
        assert(list.current_size == 0 || list.data != NULL);
        /* If allocation_size > 0, current_size should equal initial_item_allocation * item_size */
        /* and data should be non-null */
        if (initial_item_allocation > 0) {
            /* allocation succeeded, so data is non-null and current_size > 0 */
            assert(list.data != NULL);
            assert(list.current_size > 0);
            assert(list.current_size == initial_item_allocation * item_size);
        } else {
            /* No allocation needed */
            assert(list.current_size == 0);
        }
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* === FAILURE PATH assertions === */
        /* AWS_ZERO_STRUCT is called first, so list fields are zeroed */
        /* Either mul overflow or aws_mem_acquire failed */
        assert(result == AWS_OP_ERR);
        /* The list should be in a zeroed state (AWS_ZERO_STRUCT was called) */
        /* Note: on failure, the goto skips setting alloc/item_size/current_size */
        /* so they remain at their zeroed values */
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.data == NULL);
    }
}
