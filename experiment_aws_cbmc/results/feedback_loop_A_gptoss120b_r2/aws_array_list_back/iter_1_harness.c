#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Harness for aws_array_list_back */
void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocator must be a valid pointer (use default allocator) */
    list.alloc = aws_default_allocator();

    /* 2. Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* save a byte from val before the call for later comparison on failure */
    struct store_byte_from_buffer val_storage;
    save_byte_from_array((uint8_t *)val, list.item_size, &val_storage);

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save a byte from the last element (if it exists) for success check */
    struct store_byte_from_buffer last_item_storage;
    bool saved_last_item = false;
    if (list.length > 0) {
        size_t last_offset = list.item_size * (list.length - 1);
        save_byte_from_array(
            (uint8_t *)list.data + last_offset,
            list.item_size,
            &last_item_storage);
        saved_last_item = true;
    }

    /* 5. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* success implies list.length > 0, therefore we saved the last item */
        assert(saved_last_item);

        size_t last_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (uint8_t *)val,
            (uint8_t *)list.data + last_offset,
            list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* val must remain unchanged on failure */
        assert_byte_from_buffer_matches((uint8_t *)val, &val_storage);
    }

    /* 7. Fields that never change (regardless of result) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
