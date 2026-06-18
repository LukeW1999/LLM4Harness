#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
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

    /* 2. Save old state (for failure analysis) */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must be set on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);

        size_t expected_size = initial_item_allocation * item_size;
        assert(list.current_size == expected_size);
        if (expected_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
    } else {
        /* on failure the list is zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 5. Fields that never change (allocator pointer passed in) */
    /* In this function the allocator pointer is set on success and cleared on failure,
       so no additional unchanged‑field asserts are needed beyond the ones above. */

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
