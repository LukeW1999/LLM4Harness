#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    struct aws_allocator *alloc = nondet_uint8_t() ? (struct aws_allocator *)0x1 : (struct aws_allocator *)0x1;
    __CPROVER_assume(alloc != NULL);

    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Compute expected allocation size and ensure no overflow */
        size_t expected_allocation = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                              item_size,
                                              &expected_allocation);
        assert(!overflow);
        /* Fields that must change on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.current_size == expected_allocation);
        if (expected_allocation > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
        /* Unchanged fields */
        assert(list.length == 0);
    } else {
        /* On failure the list is zero‑initialized */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* (All fields are covered above; no additional unchanged fields) */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
