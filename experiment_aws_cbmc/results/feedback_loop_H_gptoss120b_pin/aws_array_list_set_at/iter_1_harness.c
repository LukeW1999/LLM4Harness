#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for val */
    void *val;
    if (list.item_size == 0) {
        val = NULL; /* zero‑size read is trivially valid */
    } else {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */

    /* Unchanged fields (allocator and item size) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Length updates */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
        /* Data pointer must be non‑NULL after a successful operation */
        assert(list.data != NULL);
    } else {
        /* On failure the length must remain unchanged */
        assert(list.length == old_length);
    }

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    return 0;
}
