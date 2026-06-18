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

/* We need to allocate an aws_string manually since it has a flexible array member */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough space for the header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Cast away const to initialize */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    /* bytes are nondet (already nondet from malloc) */
    /* ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* Non-deterministically choose whether str and buf are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    const struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 16
#endif

    if (!str_is_null) {
        str = make_arbitrary_aws_string_nondet_len_with_max(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
    }

    /* Save old state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf_ptr != NULL) ? buf_ptr->len : 0;
    size_t old_buf_capacity = (buf_ptr != NULL) ? buf_ptr->capacity : 0;
    struct aws_allocator *old_buf_allocator = (buf_ptr != NULL) ? buf_ptr->allocator : NULL;
    uint8_t *old_buf_buffer = (buf_ptr != NULL) ? buf_ptr->buffer : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions */

    /* Case: both NULL → must return true */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    }
    /* Case: exactly one NULL → must return false */
    else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    }
    /* Case: neither NULL → result depends on content comparison */
    else {
        /* If lengths differ, must be false */
        if (str->len != buf_ptr->len) {
            assert(result == false);
        }
        /* If lengths are both 0, must be true */
        if (str->len == 0 && buf_ptr->len == 0) {
            assert(result == true);
        }
        /* No further assertion on content without knowing bytes */
    }

    /* Unchanged fields: str is immutable (const fields) */
    if (str != NULL) {
        assert(str->len == old_str_len);
    }

    /* Unchanged fields: buf fields not modified */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf_len);
        assert(buf_ptr->capacity == old_buf_capacity);
        assert(buf_ptr->allocator == old_buf_allocator);
        assert(buf_ptr->buffer == old_buf_buffer);
    }

    /* Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_valid(buf_ptr));
    }
}

void aws_string_eq_byte_buf_harness(void) {
    aws_string_eq_byte_buf_harness();
    return 0;
}
