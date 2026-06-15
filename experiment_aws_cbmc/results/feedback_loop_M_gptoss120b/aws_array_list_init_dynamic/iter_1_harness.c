#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Save old state (not used for unchanged checks because function zeroes) */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic bounded inputs */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Compute the allocation size that the implementation would have computed */
        size_t allocation_size = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                              item_size,
                                              &allocation_size);
        /* Success implies no overflow */
        assert(!overflow);

        if (allocation_size > 0) {
            /* Data must have been allocated */
            assert(list.data != NULL);
            assert(list.current_size == allocation_size);
        } else {
            /* No allocation when size is zero */
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }

        /* Fields that are set on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
    } else {
        /* Failure path: the function zeroed the structure and returned error */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.length == 0);
    }

    /* 6. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
