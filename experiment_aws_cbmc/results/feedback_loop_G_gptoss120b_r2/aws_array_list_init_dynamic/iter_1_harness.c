#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs and bound them */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    /* bound the product to avoid huge allocations and overflow */
    __CPROVER_assume(item_size == 0 || initial_item_allocation <= MAX_BUFFER_SIZE / item_size);

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* multiplication must not have overflow */
        size_t expected_allocation = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation);
        assert(!overflow);

        if (expected_allocation > 0) {
            assert(list.data != NULL);
            assert(list.current_size == expected_allocation);
        } else {
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }

        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
    } else {
        /* on failure the list is zero‑initialized */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.length == 0);
    }

    /* 5. Fields that must never change regardless of result */
    /* (none besides those already asserted above) */

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
