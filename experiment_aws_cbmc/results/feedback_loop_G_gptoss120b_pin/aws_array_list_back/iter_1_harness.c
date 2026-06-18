#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state of the list */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 3. Allocate writable memory for the output value */
    size_t item_sz = list.item_size;
    void *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    struct store_byte_from_buffer val_bytes;
    save_byte_from_array((uint8_t *)val, item_sz, &val_bytes);

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);

        /* The output buffer must contain the last element */
        if (list.length > 0) {
            size_t last_offset = list.item_size * (list.length - 1);
            assert_bytes_match(
                (uint8_t *)val,
                (uint8_t *)list.data + last_offset,
                list.item_size);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);

        /* The output buffer must be unchanged */
        assert_byte_from_buffer_matches((uint8_t *)val, &val_bytes);
    }

    /* 6. Invariant that the list is always valid */
    assert(aws_array_list_is_valid(&list));
}
