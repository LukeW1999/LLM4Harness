#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable buffer for the output value */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save old bytes from the output buffer */
    struct store_byte_from_buffer val_old;
    if (val != NULL) {
        save_byte_from_array((uint8_t *)val, list.item_size, &val_old);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must contain the last element */
        size_t last_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (uint8_t *)val,
            (uint8_t *)list.data + last_offset,
            list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must be unchanged */
        if (val != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)val, &val_old);
        }
    }

    /* 7. The list validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* 8. Clean up */
    free(val);
}
