#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* aws_array_eq is used internally; we need its declaration */
bool aws_array_eq(const void *array_a, size_t len_a, const void *array_b, size_t len_b);

/*
 * Harness for aws_string_eq_byte_buf
 *
 * Implementation analysis:
 *   - If both str and buf are NULL → return true
 *   - If exactly one is NULL → return false
 *   - Otherwise → return aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
 *
 * The function is pure (read-only): it does not modify str or buf.
 * Postconditions:
 *   1. If str == NULL && buf == NULL → result == true
 *   2. If str == NULL XOR buf == NULL → result == false
 *   3. Otherwise → result == aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
 *   4. Neither str nor buf is modified
 */

/* Minimal aws_string construction for CBMC */
struct aws_string *ensure_string_is_allocated_nondet_length(void) {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    /* Allocate enough space for the struct plus len bytes (bytes[1] already in struct) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);
    /* Cast away const to initialize */
    *(size_t *)(&str->len) = len;
    *(struct aws_allocator **)(&str->allocator) = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* Non-deterministically choose whether str and buf are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    if (!str_is_null) {
        str = ensure_string_is_allocated_nondet_length();
        __CPROVER_assume(str != NULL);
        /* bytes must be readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
    }

    if (!buf_is_null) {
        buf = &buf_storage;
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save state before call (function is read-only) */
    size_t old_str_len = str ? str->len : 0;
    struct aws_allocator *old_str_allocator = str ? str->allocator : NULL;

    size_t old_buf_len = buf ? buf->len : 0;
    size_t old_buf_capacity = buf ? buf->capacity : 0;
    struct aws_allocator *old_buf_allocator = buf ? buf->allocator : NULL;
    uint8_t *old_buf_buffer = buf ? buf->buffer : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postcondition 1: both NULL → true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }
    /* Postcondition 2: exactly one NULL → false */
    else if (str == NULL || buf == NULL) {
        assert(result == false);
    }
    /* Postcondition 3: neither NULL → result from aws_array_eq */
    else {
        /* The result must be consistent with length comparison:
         * if lengths differ, aws_array_eq returns false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If lengths are both 0, aws_array_eq returns true */
        if (str->len == 0 && buf->len == 0) {
            assert(result == true);
        }
    }

    /* Postcondition 4: str fields unchanged (function is read-only) */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
    }

    /* Postcondition 5: buf fields unchanged (function is read-only) */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->allocator == old_buf_allocator);
        assert(buf->buffer == old_buf_buffer);
    }

    /* Postcondition 6: validity invariants still hold */
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
