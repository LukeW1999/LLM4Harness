#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Ensure the list can potentially succeed an erase */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.item_size > 0 && list.length > 0) {
        size_t byte_index = nondet_size_t();
        __CPROVER_assume(byte_index < list.length * list.item_size);
        save_byte_from_array((uint8_t *)list.data,
                             list.length * list.item_size,
                             &old_byte);
    }

    /* 3. Nondeterministic index (no assumptions) */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(list.length == old.length - 1);
        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* data buffer must be byte‑wise identical */
        if (list.data != NULL && list.item_size > 0 && list.length > 0) {
            assert_array_list_equivalence(&list, &old, &old_byte);
        }
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
