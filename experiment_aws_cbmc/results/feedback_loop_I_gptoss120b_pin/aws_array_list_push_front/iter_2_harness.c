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

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a value buffer (may be NULL to force failure) */
    uint8_t *val = NULL;
    bool allocate_val = nondet_bool();
    if (allocate_val && list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.length > 0 && old.data != NULL) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length increased by one */
        assert(list.length == old.length + 1);

        /* first element matches the supplied value */
        if (list.item_size > 0 && val != NULL) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)val,
                               list.item_size);
        }

        /* previous elements are shifted right */
        if (old.length > 0 && old.data != NULL) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               (uint8_t *)old.data,
                               old.length * old.item_size);
        }
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (old.length > 0 && old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 6. Frame conditions that always hold */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    /* data pointer may change on success, so no assert here */

    /* 7. Invariant that must always hold */
    assert(aws_array_list_is_valid(&list));
}
