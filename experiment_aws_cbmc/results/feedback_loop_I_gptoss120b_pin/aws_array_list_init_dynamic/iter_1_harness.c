#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_array_list_init_dynamic */
void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound the array list structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Save old state (for completeness, though it will be overwritten) */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic inputs, bounded */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Compute allocation size again to reason about the result */
        size_t allocation_size = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                             item_size,
                                             &allocation_size);
        /* Success implies no overflow */
        assert(!overflow);

        /* Allocation size determines data pointer and current_size */
        if (allocation_size > 0) {
            assert(list.data != NULL);
            assert(list.current_size == allocation_size);
        } else {
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }

        /* Fields explicitly set on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
    } else {
        /* On failure the list should be zero‑initialized */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
    }

    /* 6. Invariant: the list must always be valid */
    assert(aws_array_list_is_valid(&list));
}
