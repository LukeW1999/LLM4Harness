#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the whole structure */
    struct aws_array_list old = list;

    /* Save a byte from the data buffer for later equivalence checking */
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 2. Nondeterministic index and path selection */
    size_t idx = nondet_size_t();
    bool force_success = nondet_bool();

    if (force_success) {
        /* Force the success path: list must have at least one element and idx within bounds */
        __CPROVER_assume(list.length > 0);
        __CPROVER_assume(idx < list.length);
    } else {
        /* Force the failure path: either list empty or idx out of bounds */
        __CPROVER_assume(!(list.length > 0 && idx < list.length));
    }

    /* 3. Call function under test */
    int result = aws_array_list_erase(&list, idx);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must decrease by one */
        assert(list.length == old.length - 1);

        /* Allocator, item size, and data pointer must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* current_size (bytes allocated) does not change on erase */
        assert(list.current_size == old.current_size);

        /* The contents of the list are the same as the old list
         * except for the element removed (and the possible shift of later elements). */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* On failure the entire list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 5. The list must remain valid in all cases */
    assert(aws_array_list_is_valid(&list));
}
