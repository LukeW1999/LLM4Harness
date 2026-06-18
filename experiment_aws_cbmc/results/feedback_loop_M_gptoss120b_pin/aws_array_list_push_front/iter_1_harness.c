#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a nondet value buffer of size list->item_size */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_first_byte = {0};
    if (old.length > 0) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_first_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* allocator and item size stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* first element must equal the supplied value */
        assert_bytes_match((uint8_t *)list.data, val, list.item_size);
        /* previously first element (if any) is now second */
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               (uint8_t *)old.data,
                               list.item_size);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data,
                                            &old_first_byte);
        }
    }

    /* 6. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
