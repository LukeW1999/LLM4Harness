#include <stddef.h>
#include <assert.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable buffer for the output value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    void *val = (void *)val_buf;

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save old contents of the output buffer */
    struct store_byte_from_buffer val_old;
    save_byte_from_array(val_buf, list.item_size, &val_old);

    /* 5. If the list is non‑empty, save the last element */
    struct store_byte_from_buffer last_elem;
    bool has_last = false;
    if (list.length > 0) {
        size_t last_offset = list.item_size * (list.length - 1);
        uint8_t *last_ptr = (uint8_t *)list.data + last_offset;
        save_byte_from_array(last_ptr, list.item_size, &last_elem);
        has_last = true;
    }

    /* 6. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* success: the output buffer must contain the last element */
        assert(has_last);
        size_t last_offset = list.item_size * (list.length - 1);
        uint8_t *last_ptr = (uint8_t *)list.data + last_offset;
        assert_bytes_match(last_ptr, val_buf, list.item_size);
    } else {
        /* failure (list empty): the output buffer must be unchanged */
        assert(!has_last);
        assert_byte_from_buffer_matches(val_buf, &val_old);
    }

    /* 8. Fields that must remain unchanged regardless of the result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 9. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
