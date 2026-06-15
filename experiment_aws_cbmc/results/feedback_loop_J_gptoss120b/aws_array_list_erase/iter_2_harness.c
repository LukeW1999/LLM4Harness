#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(list.length == old.length - 1);

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        /* data before the erased element must be unchanged */
        if (list.data != NULL && index <= old.length) {
            size_t prefix_bytes = index * list.item_size;
            if (prefix_bytes > 0) {
                assert_bytes_match((uint8_t *)list.data,
                                   (uint8_t *)old.data,
                                   prefix_bytes);
            }
        }

        /* bytes after the new end must remain unchanged */
        size_t old_total_bytes = old.length * list.item_size;
        size_t new_total_bytes = list.length * list.item_size;
        if (old_total_bytes > new_total_bytes) {
            size_t suffix_bytes = old_total_bytes - new_total_bytes;
            assert_bytes_match((uint8_t *)list.data + new_total_bytes,
                               (uint8_t *)old.data + new_total_bytes,
                               suffix_bytes);
        }
    } else {
        /* on failure the whole list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

        if (list.data != NULL && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
