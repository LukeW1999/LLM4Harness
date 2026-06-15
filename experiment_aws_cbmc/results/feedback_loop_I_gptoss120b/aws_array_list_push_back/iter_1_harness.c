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

    /* 2. Allocate a readable input buffer of size list.item_size */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* nondet fill */
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));

        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length updates only when index is beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* data pointer must be non‑NULL after a successful set */
        assert(list.data != NULL);

        /* the stored element must equal the input value */
        assert(index < list.length); /* safety for pointer arithmetic */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* validity invariant must still hold */
        assert(aws_array_list_is_valid(&list));
    }

    /* clean up */
    free(val);
}
