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
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    void *val = malloc(list.item_size ? list.item_size : 1);
    __CPROVER_assume(val != NULL);
    /* Assume the buffer is writable for list->item_size bytes */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save old bytes of the output buffer */
    struct store_byte_from_buffer val_old_storage;
    save_byte_from_array((uint8_t *)val, list.item_size, &val_old_storage);

    /* 5. If the list is non‑empty, save the bytes of the last element */
    struct store_byte_from_buffer last_item_storage;
    bool list_nonempty = (list.length > 0);
    if (list_nonempty) {
        size_t last_offset = list.item_size * (list.length - 1);
        save_byte_from_array((uint8_t *)list.data + last_offset,
                             list.item_size,
                             &last_item_storage);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */

    /* 7a. Success path */
    if (result == AWS_OP_SUCCESS) {
        /* The output buffer now contains a copy of the last element */
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)&last_item_storage.bytes,
                           list.item_size);
        /* The list itself must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Failure path: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* The output buffer must remain unchanged */
        assert_byte_from_buffer_matches((uint8_t *)val, &val_old_storage);
    }

    /* 8. Invariant that must always hold */
    assert(aws_array_list_is_valid(&list));
}
