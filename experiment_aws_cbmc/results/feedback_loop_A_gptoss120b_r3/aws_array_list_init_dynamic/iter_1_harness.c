#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare inputs and bound them */
    struct aws_array_list list;
    /* nondet initial allocation count and item size */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* allocator must be non‑NULL */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* 2. Save old state (not really needed for init, but kept for pattern) */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Compute expected allocation size and overflow flag using the same helper */
    size_t expected_allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation_size);

    /* 4. Postconditions for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* success implies no overflow */
        assert(!overflow);

        /* item size is stored */
        assert(list.item_size == item_size);

        /* allocator is stored */
        assert(list.alloc == alloc);

        /* length is zero after init */
        assert(list.length == 0);

        /* current_size and data correspond to the allocation */
        if (expected_allocation_size > 0) {
            assert(list.current_size == expected_allocation_size);
            assert(list.data != NULL);
        } else {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        }

        /* postcondition from implementation */
        assert(list.current_size == 0 || list.data);
    } else {
        /* on error the list remains zeroed */
        assert(list.alloc == NULL);
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.item_size == 0);
        assert(list.length == 0);
    }

    /* 5. Fields that must never change regardless of result (none besides those already asserted) */

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
