#include <stdlib.h>
#include <string.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state of the list */
    struct aws_array_list old = list;

    /* 3. Allocate writable memory for the output value */
    void *val = malloc(list.item_size ? list.item_size : 1);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 4. Save old bytes of val (to check unchanged on failure) */
    struct store_byte_from_buffer old_val_storage;
    save_byte_from_array((uint8_t *)val, list.item_size, &old_val_storage);

    /* 5. If list is non‑empty, save the expected last element bytes */
    struct store_byte_from_buffer expected_storage;
    if (list.length > 0) {
        uint8_t *last_elem = (uint8_t *)list.data +
                             list.item_size * (list.length - 1);
        save_byte_from_array(last_elem, list.item_size, &expected_storage);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* val must contain the last element */
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data +
                           list.item_size * (list.length - 1),
                           list.item_size);
        /* list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* val must be unchanged */
        assert_byte_from_buffer_matches((uint8_t *)val, &old_val_storage);
        /* list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 8. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
