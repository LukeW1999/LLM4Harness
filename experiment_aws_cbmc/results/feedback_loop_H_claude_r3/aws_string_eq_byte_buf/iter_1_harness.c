#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_eq_byte_buf
 *
 * From the implementation:
 *   - If both str and buf are NULL, returns true
 *   - If exactly one is NULL, returns false
 *   - Otherwise, returns aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
 *
 * Neither str nor buf are modified by this function.
 * The function is a pure comparison — no fields change.
 */

/* We need a helper to allocate an aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    /* Initialize fields — cast away const for initialization purposes */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    /* bytes are non-deterministic (already set by malloc in CBMC) */
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs non-deterministically */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    /* Set up str */
    if (!str_is_null) {
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif
        str = ensure_string_is_allocated(MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up buf */
    if (!buf_is_null) {
        buf = &buf_storage;
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* 2. Save old state (nothing changes, but we verify immutability) */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* 4. Assert postconditions */

    /* Case: both NULL → must return true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }
    /* Case: exactly one NULL → must return false */
    else if (str == NULL || buf == NULL) {
        assert(result == false);
    }
    /* Case: neither NULL → result depends on content comparison */
    else {
        /* If lengths differ, must return false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If lengths are both 0, must return true */
        if (str->len == 0 && buf->len == 0) {
            assert(result == true);
        }
        /* result is either true or false — no further constraint we can assert
           without re-implementing the comparison */
    }

    /* 5. Assert unchanged fields — str is immutable */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        assert(aws_string_is_valid(str));
    }

    /* 6. Assert unchanged fields — buf is immutable */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->allocator == old_buf_allocator);
        assert(buf->buffer == old_buf_buffer);
        assert(aws_byte_buf_is_valid(buf));
    }
}
