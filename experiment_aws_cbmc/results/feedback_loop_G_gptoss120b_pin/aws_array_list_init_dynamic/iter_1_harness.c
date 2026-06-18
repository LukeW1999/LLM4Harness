#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare data structures and nondet inputs */
    struct aws_array_list list;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* 2. Bound nondet inputs */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Bound the list structure (pre‑state) */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 4. Use the default allocator (required by the API) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 5. Save old state for later comparison */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must be set on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        if (list.current_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        /* on failure the list is left zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 8. Fields that never change regardless of outcome */
    /* (none besides those already asserted above) */

    /* 9. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
