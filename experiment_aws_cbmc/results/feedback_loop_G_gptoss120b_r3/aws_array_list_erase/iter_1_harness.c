#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index */
    size_t idx = nondet_size_t();

    /* 3. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, idx);

    /* 5. Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* index must have been within bounds */
        assert(idx < old_length);
        /* length decreased by exactly one */
        assert(list.length == old_length - 1);
        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* on failure the list is unchanged */
        assert(list.length == old_length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
