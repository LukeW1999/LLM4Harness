#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <stddef.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the buffer readable – nondet contents are fine */
    (void)val; /* silence unused warning */

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may increase if index was beyond the previous length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* allocator and item size never change */
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);

        /* current_size (capacity) may grow but never shrink */
        assert(list.current_size >= old_current_size);

        /* data pointer may change due to reallocation – no assertion on equality */

    } else {
        /* On failure the structural fields must remain unchanged */
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    /* 7. Invariant must hold regardless of outcome */
    assert(aws_array_list_is_valid(&list));

    /* 8. Clean up */
    free(val);
}
