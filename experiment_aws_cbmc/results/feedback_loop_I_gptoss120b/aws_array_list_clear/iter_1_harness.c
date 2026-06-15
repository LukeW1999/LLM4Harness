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
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the memory readable – nondet contents are fine */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a nondeterministic index, bounded to avoid overflow in pointer arithmetic */
    size_t index = nondet_size_t();
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_data);
    }

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

        /* the item at the given index now matches the source value */
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size and data may change because of reallocation, so we do not assert them */
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. The validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
