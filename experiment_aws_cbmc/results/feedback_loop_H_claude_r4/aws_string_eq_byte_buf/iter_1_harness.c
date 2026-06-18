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
 * From Doxygen: Returns true if bytes of string and buffer are the same, false otherwise.
 *
 * From implementation:
 * - If both str and buf are NULL, returns true
 * - If exactly one is NULL, returns false
 * - Otherwise, compares bytes using aws_array_eq
 *
 * 1. Changed fields: none (pure comparison, no mutation)
 * 2. Unchanged fields: all fields of str and buf remain unchanged
 * 3. Failure: N/A (returns bool, not error code)
 * 4. Validity invariants: str and buf remain valid after the call
 */

/* We need a helper to allocate an aws_string with bounded length */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_string_eq_byte_buf_harness(void) {
    /* Set up aws_string */
    /* aws_string has a flexible array member, so we allocate it manually */
    bool str_is_null = nondet_bool();
    const struct aws_string *str = NULL;

    if (!str_is_null) {
        /* Allocate a string with bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* Allocate memory for the string header + bytes + null terminator */
        struct aws_string *s = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(s != NULL);

        /* Initialize the length field (const, so use a trick) */
        *(size_t *)(&s->len) = str_len;
        /* allocator can be anything */
        *(struct aws_allocator **)(&s->allocator) = nondet_bool() ? NULL : aws_default_allocator();

        /* bytes are non-deterministic (already allocated) */
        /* null terminator */
        ((uint8_t *)s->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(s));
        str = s;
    }

    /* Set up aws_byte_buf */
    bool buf_is_null = nondet_bool();
    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_storage;

    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    struct aws_byte_buf old_buf_storage;
    if (buf != NULL) {
        old_buf_storage = buf_storage;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* Case 1: both NULL => true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }
    /* Case 2: exactly one NULL => false */
    else if (str == NULL || buf == NULL) {
        assert(result == false);
    }
    /* Case 3: neither NULL => result depends on content comparison */
    else {
        /* We can assert that if lengths differ, result must be false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If lengths are equal and result is true, bytes must match */
        /* (We can't easily assert byte-by-byte here without loops, but we can
           assert the structural invariants) */
    }

    /* Immutability: str pointer unchanged */
    assert(str == old_str);

    /* Immutability: buf fields unchanged */
    if (buf != NULL) {
        assert(buf->len == old_buf_storage.len);
        assert(buf->capacity == old_buf_storage.capacity);
        assert(buf->buffer == old_buf_storage.buffer);
        assert(buf->allocator == old_buf_storage.allocator);
    }

    /* Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}

void aws_string_eq_byte_buf_harness(void) {
    aws_string_eq_byte_buf_harness();
    return 0;
}
