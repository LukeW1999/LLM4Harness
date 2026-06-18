#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL */
    bool src_is_null;
    __CPROVER_assume(src_is_null == 0 || src_is_null == 1);

    const struct aws_string *src = NULL;

    if (!src_is_null) {
        /* Use the proof helper to allocate a valid aws_string */
        src = nondet_const_aws_string_ptr();
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Snapshot src fields before the call (for frame conditions) */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? src->bytes : NULL;

    /* Save a byte from the string for immutability check */
    struct store_byte_from_buffer saved_byte;
    bool saved = false;
    if (src != NULL && src->len > 0) {
        save_byte_from_array(src->bytes, src->len, &saved_byte);
        saved = true;
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* --- Postconditions --- */

    if (src == NULL) {
        /* NULL input: must return empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Non-NULL input: cursor must point to string bytes with correct length */
        assert(result.ptr == (uint8_t *)src->bytes);
        assert(result.len == src->len);

        /* FRAME: src must not have been modified */
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);

        /* Immutability: bytes content unchanged */
        if (saved) {
            assert_byte_from_buffer_matches(src->bytes, &saved_byte);
        }

        /* cursor length matches string length */
        assert(result.len == old_len);
        /* if len > 0, ptr must not be NULL */
        if (result.len > 0) {
            assert(result.ptr != NULL);
        }
    }

    /* INVARIANTS: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
