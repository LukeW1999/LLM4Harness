#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    size_t val_size = list.item_size ? list.item_size : 1;
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);
    /* precondition: writable memory of list->item_size bytes */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save a copy of the output buffer for failure case */
    void *old_val = malloc(val_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, val_size);

    /* 5. If list is non‑empty, save the last element bytes */
    struct store_byte_from_buffer last_elem_storage;
    size_t last_offset = 0;
    if (list.length > 0) {
        last_offset = list.item_size * (list.length - 1);
        save_byte_from_array(
            (const uint8_t *)list.data + last_offset,
            list.item_size,
            &last_elem_storage);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* output buffer must contain the previously last element */
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)list.data + last_offset,
            list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* output buffer must be unchanged */
        assert_bytes_match(
            (const uint8_t *)val,
            (const uint8_t *)old_val,
            val_size);
    }

    /* 8. Invariant: the list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(old_val);
}
