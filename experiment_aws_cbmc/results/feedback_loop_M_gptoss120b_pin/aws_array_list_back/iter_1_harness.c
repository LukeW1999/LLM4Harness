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

void aws_array_list_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Allocate writable output buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must have been > 0 */
        assert(list.length > 0);
        /* the output buffer must contain the last element */
        size_t last_offset = list.item_size * (list.length - 1);
        assert_bytes_match(
            (uint8_t *)val,
            (uint8_t *)list.data + last_offset,
            list.item_size);
    } else {
        /* failure occurs only when the list is empty */
        assert(list.length == 0);
    }

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Immutability of underlying data buffer */
    if (list.data != NULL && list.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
