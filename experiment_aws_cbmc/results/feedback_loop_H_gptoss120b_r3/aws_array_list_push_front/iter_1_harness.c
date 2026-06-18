#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source value of size list->item_size */
    uint8_t *src = NULL;
    if (list.item_size > 0) {
        src = malloc(list.item_size);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(src, list.item_size));
    }
    const void *val = src;

    /* 3. Save old state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data_byte);
    }

    /* 4. Call the function under test */
    int err = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (err == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);

        /* item_size, allocator, and item_size must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* first element must equal the supplied value */
        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (const uint8_t *)val,
                               list.item_size);
        }

        /* existing elements (if any) must be shifted correctly */
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               (uint8_t *)old.data,
                               old.length * list.item_size);
        }

        /* current_size may have changed (capacity growth), data pointer may have changed;
           no assertions required for those fields. */
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_data_byte);
    }

    /* 6. Fields that never change regardless of success/failure */
    /* allocator and item_size are already asserted above for both paths */
    /* No other fields are guaranteed to stay unchanged. */

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free((void *)src);
}
