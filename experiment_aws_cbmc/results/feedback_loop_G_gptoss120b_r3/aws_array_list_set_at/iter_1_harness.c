#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value */
    /* item_size may be zero, but the function requires a non‑zero size for the copy.
       Constrain it to be non‑zero for the harness. */
    __CPROVER_assume(list.item_size > 0);
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the source buffer readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_val_bytes;
    save_byte_from_array(val, list.item_size, &old_val_bytes);

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* the element at the given index must now contain the bytes from val */
        assert_bytes_match(
            (uint8_t *)list.data + (list.item_size * index),
            val,
            list.item_size);

        /* length handling */
        if (index >= old.length) {
            /* length should become index+1 (no overflow) */
            assert(list.length == index + 1);
        } else {
            /* length must stay the same */
            assert(list.length == old.length);
        }

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
