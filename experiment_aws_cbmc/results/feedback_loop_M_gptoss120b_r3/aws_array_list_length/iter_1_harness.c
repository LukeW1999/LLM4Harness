#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable buffer for the value to be set */
    size_t item_sz = list.item_size ? list.item_size : 1;
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    /* make the buffer readable for CBMC */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* 3. Non‑deterministic index, bounded to keep state space finite */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may grow if we wrote past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the stored element must equal the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may increase in dynamic mode, so we do not assert it unchanged */
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 7. Invariant: the list is always valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
