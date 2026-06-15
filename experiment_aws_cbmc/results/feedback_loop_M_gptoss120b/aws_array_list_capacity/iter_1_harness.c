#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare nondeterministic inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Allocate a buffer for the value to be copied */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* The contents of val are nondeterministic; no need to initialise */
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase to index+1 if index was beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must not change on success */
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* current_size may change (dynamic growth), so no assertion */
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
    }

    /* 6. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
