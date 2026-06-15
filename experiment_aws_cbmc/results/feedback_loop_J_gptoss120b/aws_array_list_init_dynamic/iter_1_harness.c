#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare data structures and inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* 2. Apply precondition assumptions */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Save old state before the call */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(&list, alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Compute the expected allocation size using the same checked multiplication */
        size_t expected_allocation = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                             item_size,
                                             &expected_allocation);

        /* Success implies no overflow */
        assert(!overflow);

        /* Fields that must be set on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.current_size == expected_allocation);
        assert(list.length == 0);

        if (expected_allocation > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
    } else {
        /* On failure the function zeroes the structure */
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.data == NULL);
    }

    /* 6. Invariant that must always hold */
    assert(aws_array_list_is_valid(&list));
}
