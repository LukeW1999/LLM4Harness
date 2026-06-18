#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Harness for aws_string_eq_byte_buf
 *
 * From the implementation:
 *   - If both str and buf are NULL, returns true
 *   - If exactly one is NULL, returns false
 *   - Otherwise, returns aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
 *     which compares the bytes of the string with the bytes of the buffer
 *
 * Neither input is modified by this function.
 */

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Set up aws_string (possibly NULL) */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate a bounded aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the length field (const, so use a cast trick) */
        *(size_t *)(&str->len) = str_len;

        /* The allocator can be anything (including NULL for static strings) */
        /* bytes are non-deterministic (already allocated) */

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Set up aws_byte_buf (possibly NULL) */
    struct aws_byte_buf *buf = NULL;
    bool buf_is_null = nondet_bool();

    struct aws_byte_buf local_buf;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&local_buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&local_buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&local_buf));
        buf = &local_buf;
    }

    /* 3. Snapshot inputs before the call */
    /* Snapshot str fields */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    /* Snapshot buf fields */
    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;

    /* 4. Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 5. Assert postconditions */

    /* --- Return value postconditions --- */
    if (str == NULL && buf == NULL) {
        /* Both NULL => true */
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        /* Exactly one NULL => false */
        assert(result == false);
    }
    /* When neither is NULL, result depends on byte comparison — no further constraint */

    /* --- Frame: str must not be modified --- */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        /* aws_string_is_valid still holds */
        assert(aws_string_is_valid(str));
    }

    /* --- Frame: buf must not be modified --- */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->buffer == old_buf_buffer);
        assert(buf->allocator == old_buf_allocator);
        /* aws_byte_buf_is_valid still holds */
        assert(aws_byte_buf_is_valid(buf));
    }
}
