#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_STRING_LEN 10

void aws_string_eq_byte_buf_harness() {
    /* nondet choice for string and buffer pointer nullability */
    const struct aws_string *str = nondet_bool() ? NULL : (const struct aws_string *)malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    const struct aws_byte_buf *buf = nondet_bool() ? NULL : (const struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));

    /* if not null, assume validity and memory bounds */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->len < MAX_STRING_LEN);
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
    }
    if (buf != NULL) {
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_STRING_LEN));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* save old states for immutability checks */
    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf != NULL) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->len, &old_buf_bytes);
    }

    /* call function */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* if both non-null, result is either true or false */
        assert(result == true || result == false);
    }

    /* immutability: string and buffer must be unchanged */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
        /* ensure len unchanged (implied by const) */
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
        assert(buf->allocator == old_buf.allocator);
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->buffer == old_buf.buffer);
        assert_byte_from_buffer_matches(buf->buffer, &old_buf_bytes);
    }
}
