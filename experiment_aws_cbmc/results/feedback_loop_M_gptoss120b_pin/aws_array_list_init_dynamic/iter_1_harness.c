#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs and bound them */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state (for failure case) */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Compute expected allocation size using the same checked multiplication */
    size_t allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);

    /* 4. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == allocation_size);
        if (allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        /* Failure: list should remain zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
        /* Ensure no unexpected modifications compared to old (which is also zero‑initialized) */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 5. Fields that never change regardless of outcome (none beyond those already asserted) */

    /* 6. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
