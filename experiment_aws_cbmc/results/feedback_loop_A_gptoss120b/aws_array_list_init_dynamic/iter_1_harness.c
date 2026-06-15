#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocator (non‑NULL) */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* nondet inputs with bounds */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must change on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);

        size_t expected_size = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                              item_size,
                                              &expected_size);
        /* overflow must be false on the success path */
        assert(!overflow);
        assert(list.current_size == expected_size);
        if (expected_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
        /* postcondition from implementation */
        assert(list.current_size == 0 || list.data);
    } else {
        /* on failure the list remains zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No additional fields remain unchanged beyond those already asserted */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
