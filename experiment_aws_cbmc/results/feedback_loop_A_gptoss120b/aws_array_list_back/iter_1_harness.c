#include <stdlib.h>
#include <assert.h>
#include "aws/common/array_list.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* Save a copy of the last element (if any) for later comparison */
    struct store_byte_from_buffer last_item_storage;
    if (list.length > 0) {
        size_t last_item_offset = list.item_size * (list.length - 1);
        save_byte_from_array((const uint8_t *)list.data + last_item_offset,
                             list.item_size,
                             &last_item_storage);
    }

    /* Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* 3. Call function under test */
    int result = aws_array_list_back(&list, val);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The output buffer must contain the last element */
        if (list.length > 0) {
            size_t last_item_offset = list.item_size * (list.length - 1);
            assert_bytes_match((const uint8_t *)val,
                               (const uint8_t *)old.data + last_item_offset,
                               list.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* No guarantee about *val on failure */
    }

    /* 5. Invariant that must always hold */
    assert(aws_array_list_is_valid(&list));
}
