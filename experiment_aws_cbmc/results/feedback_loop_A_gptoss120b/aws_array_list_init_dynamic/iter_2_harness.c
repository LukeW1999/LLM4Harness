#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_dynamic_harness() {
    /* 1. Zero‑initialize the list */
    struct aws_array_list list = {0};

    /* 2. Use a valid allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Nondet inputs with bounds */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 4. Save old state (not used but kept for symmetry) */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);

        size_t expected_size = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                              item_size,
                                              &expected_size);
        assert(!overflow);
        assert(list.current_size == expected_size);
        if (expected_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
        assert(list.current_size == 0 || list.data);
    } else {
        /* on failure the list remains zero‑initialized */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 7. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
