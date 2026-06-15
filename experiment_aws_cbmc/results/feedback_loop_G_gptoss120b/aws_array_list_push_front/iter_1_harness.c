#include <stdlib.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a nondeterministic input value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* (the memory pointed to by val is readable for list->item_size bytes) */

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.length > 0) {
        save_byte_from_array(
            (uint8_t *)old.data,
            old.length * old.item_size,
            &old_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields */
        assert(list.length == old.length + 1);
        assert_bytes_match(
            (uint8_t *)list.data,
            val,
            list.item_size);
        if (old.length > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + list.item_size,
                (uint8_t *)old.data,
                old.length * old.item_size);
        }

        /* Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (old.length > 0) {
            assert_byte_from_buffer_matches(
                (uint8_t *)list.data,
                &old_byte);
        }
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
