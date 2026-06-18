#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>          /* for aws_default_allocator() */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_array_list_init_dynamic */
void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state (for completeness, though not used in assertions) */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Compute expected allocation size and overflow flag */
        size_t expected_allocation = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation);
        /* Success implies no overflow */
        assert(!overflow);

        /* Fields that must be set on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);                     /* zeroed by AWS_ZERO_STRUCT */

        if (expected_allocation > 0) {
            assert(list.data != NULL);
            assert(list.current_size == expected_allocation);
        } else {
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }
    } else {
        /* On failure the list remains zeroed */
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
    }

    /* 5. Invariant: the list must be valid regardless of outcome */
    assert(aws_array_list_is_valid(&list));
}
