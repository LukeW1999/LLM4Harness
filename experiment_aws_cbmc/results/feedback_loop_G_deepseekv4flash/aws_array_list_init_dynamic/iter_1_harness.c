#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness() {
    /* Non-deterministic parameters */
    struct aws_array_list list;
    struct aws_allocator *alloc = malloc(sizeof(struct aws_allocator));
    size_t initial_item_allocation;
    size_t item_size;

    /* Assume valid parameters */
    __CPROVER_assume(alloc != NULL);
    __CPROVER_assume(item_size > 0);

    /* Bound the sizes to avoid excessive state space */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Call the function */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, fields are set as specified by the function */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == (initial_item_allocation * item_size)); // No overflow because we bounded

        /* If allocation_size > 0, data must be non-null and writable.
         * The allocation is modeled by aws_mem_acquire, which we assume returns valid memory.
         * We only know that if current_size > 0, data is not null.
         */
        if (list.current_size > 0) {
            assert(list.data != NULL);
            assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        } else {
            assert(list.data == NULL);
        }
    } else {
        /* On failure, the list is zeroed (zeroed at start, then error returns immediately) */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* Validity invariants must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
