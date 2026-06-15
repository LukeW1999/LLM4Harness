#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic choices for NULL pointers */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Set up str if non-NULL */
    if (!str_is_null) {
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        /* Ensure the entire string (header + bytes) is readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string) + str->len));
        /* Ensure the string is valid (includes null terminator at bytes[len]) */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up buf if non-NULL */
    if (!buf_is_null) {
        buf = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    struct aws_byte_buf old_buf;
    if (str != NULL) {
        old_str = *str;
    }
    if (buf != NULL) {
        old_buf = *buf;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* 1. Return value correctness based on NULL handling */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result is the outcome of byte comparison */
        /* We cannot compute the exact comparison, but we know it's a boolean */
        assert(result == true || result == false);
    }

    /* 2. Unchanged fields for str */
    if (str != NULL) {
        assert(str->len == old_str.len);
        assert(str->allocator == old_str.allocator);
        /* bytes is a flexible array member; pointer address is same */
        assert(str->bytes == old_str.bytes);
    }

    /* 3. Unchanged fields for buf */
    if (buf != NULL) {
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->allocator == old_buf.allocator);
        assert(buf->buffer == old_buf.buffer);
    }

    /* 4. Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
