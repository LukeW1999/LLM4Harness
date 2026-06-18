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

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate a readable buffer for val */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.length > 0 && old.data != NULL) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_byte);
    }

    /* 3. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* length increased by one */
        assert(list.length == old.length + 1);

        /* first element matches the supplied value */
        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)val,
                               list.item_size);
        }

        /* previous elements are shifted right */
        if (old.length > 0) {
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

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    /* data pointer may change on success (reallocation), so we do not assert it here */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
