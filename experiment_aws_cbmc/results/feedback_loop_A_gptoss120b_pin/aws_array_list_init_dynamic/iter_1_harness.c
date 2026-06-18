#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* Allocator (default) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Non‑deterministic inputs, bounded */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(item_size > 0); /* precondition */

    /* Compute expected allocation size and overflow flag */
    size_t expected_allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation,
                                          item_size,
                                          &expected_allocation_size);

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success guarantees */
        assert(!overflow);
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == expected_allocation_size);
        if (expected_allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
        /* Postcondition from implementation */
        assert(list.current_size == 0 || list.data);
    } else {
        /* Failure: list should be zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 5. Unchanged fields (none remain unchanged beyond those asserted above) */

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
