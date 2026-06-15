#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_STRING_LEN 100

void aws_string_eq_byte_buf_harness() {
    /* non-deterministic choices for NULL pointers */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* set up str if non-NULL */
    if (!str_is_null) {
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);
        /* bytes is a flexible array member; ensure it is allocated */
        /* Explicitly set bytes to a valid buffer */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string) + str->len));
        /* Ensure null-terminator at bytes[len] */
        str->bytes[str->len] = '\0';
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* set up buf if non-NULL */
    if (!buf_is_null) {
        buf = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* save old state */
    struct aws_string old_str;
    struct aws_byte_buf old_buf;
    if (str != NULL) {
        old_str = *str;
    }
    if (buf != NULL) {
        old_buf = *buf;
    }

    /* call the function */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* postconditions */

    /* 1. NULL handling */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* both non-NULL: result is boolean */
        assert(result == true || result == false);
    }

    /* 2. immutability of str */
    if (str != NULL) {
        assert(str->len == old_str.len);
        assert(str->allocator == old_str.allocator);
        /* flexible array member pointer (address of first byte) unchanged */
        assert(str->bytes == old_str.bytes);
    }

    /* 3. immutability of buf */
    if (buf != NULL) {
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->allocator == old_buf.allocator);
        assert(buf->buffer == old_buf.buffer);
    }

    /* 4. validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
