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
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate writable output buffer */
    size_t item_sz = list.item_size;
    /* item_sz is guaranteed > 0 by validity predicate */
    void *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state of the list */
    struct aws_array_list old = list;

    /* 4. Save original contents of val (for failure case) */
    struct store_byte_from_buffer val_old;
    save_byte_from_array((uint8_t *)val, item_sz, &val_old);

    /* 5. If list has elements, save the last element for later comparison */
    struct store_byte_from_buffer last_elem;
    size_t last_offset = 0;
    if (list.length > 0) {
        last_offset = list.item_size * (list.length - 1);
        save_byte_from_array(
            (uint8_t *)list.data + last_offset,
            list.item_size,
            &last_elem);
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

        /* val must now contain the previously saved last element */
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

        /* val must be unchanged */
        assert_byte_from_buffer_matches(val, &val_old);
    }

    /* 8. Invariant: the list remains valid */
    assert(aws_array_list_is_valid(&list));
}
