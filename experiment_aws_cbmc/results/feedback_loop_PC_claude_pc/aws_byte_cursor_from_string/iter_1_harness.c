#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_cursor_from_string.
 *
 * From the Doxygen:
 *   "Creates an aws_byte_cursor from an existing string.
 *    If the src is NULL, it returns an empty cursor."
 *
 * From the implementation:
 *   - If src == NULL: returns a zero-initialized cursor (ptr=NULL, len=0).
 *   - If src != NULL: returns aws_byte_cursor_from_array(aws_string_bytes(src), src->len),
 *     i.e., cursor.ptr == src->bytes, cursor.len == src->len.
 *
 * Postconditions:
 *   1. RETURN: cursor.ptr and cursor.len match expectations based on src.
 *   2. FRAME: src (the string) is not modified.
 *   3. INVARIANTS: returned cursor is valid.
 */

/* Helper: allocate a valid aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);

    /* Allocate enough memory for the struct header + len bytes + 1 null terminator */
    size_t alloc_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(alloc_size);
    __CPROVER_assume(str != NULL);

    /* Initialize the const fields via a writable pointer trick */
    /* We cast away const to initialize in the harness */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;

    /* allocator can be NULL or non-NULL */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = NULL; /* static-style string is fine for this test */

    /* bytes are already non-deterministic from malloc */
    /* Ensure null terminator is present */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';

    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL */
    bool src_is_null = nondet_bool();

    const struct aws_string *src = NULL;

    if (!src_is_null) {
        /* Allocate a valid aws_string */
        size_t max_len = 16; /* bound the state space */
        src = ensure_string_is_allocated(max_len);
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Snapshot src fields before the call (for frame conditions) */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? src->bytes : NULL;
    struct aws_allocator *old_allocator = (src != NULL) ? src->allocator : NULL;

    /* Save a byte from the string for immutability check */
    struct store_byte_from_buffer saved_byte;
    if (src != NULL && src->len > 0) {
        save_byte_from_array(src->bytes, src->len, &saved_byte);
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
        assert(result.ptr == src->bytes);
        assert(result.len == src->len);

        /* FRAME: src must not have been modified */
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
        assert(src->allocator == old_allocator);

        /* Immutability: bytes content unchanged */
        if (src->len > 0) {
            assert_byte_from_buffer_matches(src->bytes, &saved_byte);
        }
    }

    /* INVARIANTS: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* Additional consistency checks */
    if (src != NULL) {
        /* cursor length matches string length */
        assert(result.len == old_len);
        /* if len > 0, ptr must not be NULL */
        if (result.len > 0) {
            assert(result.ptr != NULL);
        }
    }
}

void aws_byte_cursor_from_string_harness(void) {
    aws_byte_cursor_from_string_harness();
    return 0;
}
