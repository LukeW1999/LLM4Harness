#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministically decide whether the list is zeroed or a valid list */
    bool make_zeroed = __CPROVER_nondet_bool();

    if (make_zeroed) {
        /* Zeroed list */
        AWS_ZERO_STRUCT(list);
    } else {
        /* Build a valid list */
        size_t item_size = __CPROVER_nondet_size_t();
        __CPROVER_assume(item_size > 0);

        /* Choose a capacity (number of items) */
        size_t capacity = __CPROVER_nondet_size_t();
        __CPROVER_assume(capacity <= 1024); /* bound for CBMC */

        /* Compute current_size = capacity * item_size, checking overflow */
        size_t current_size = 0;
        __CPROVER_assume(!aws_mul_size_checked(capacity, item_size, &current_size));

        /* Allocate data buffer */
        uint8_t *data = NULL;
        if (current_size > 0) {
            data = (uint8_t *)aws_mem_acquire(alloc, current_size);
            __CPROVER_assume(data != NULL);
            /* Fill with nondet data */
            __CPROVER_assume(__CPROVER_is_fresh(data, current_size));
        }

        /* Choose a length (number of initialized items) */
        size_t length = __CPROVER_nondet_size_t();
        __CPROVER_assume(length <= capacity);

        /* Populate the list struct */
        list.alloc = alloc;
        list.item_size = item_size;
        list.current_size = current_size;
        list.length = length;
        list.data = data;

        /* Ensure the list satisfies its invariant */
        __CPROVER_assume(aws_array_list_is_valid(&list));
    }

    /* Preserve original fields for frame condition checks */
    struct aws_allocator *orig_alloc = list.alloc;
    size_t orig_item_size = list.item_size;
    size_t orig_current_size = list.current_size;
    void *orig_data = list.data;

    /* Call the function under test */
    aws_array_list_clear(&list);

    /* Postcondition: either the list is zeroed or remains valid */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Length must be zero after clear (if data was present) */
    if (list.data != NULL) {
        assert(list.length == 0);
    }

    /* Frame conditions: fields other than length should be unchanged */
    assert(list.alloc == orig_alloc);
    assert(list.item_size == orig_item_size);
    assert(list.current_size == orig_current_size);
    assert(list.data == orig_data);

    return 0;
}
