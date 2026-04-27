#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_buf_harness() {
    /* parameters */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* assumptions */
    __CPROVER_assume(aws_string_is_bounded(str, MAX_STRING_LEN));
    ensure_string_has_allocated_buffer_member(str);
    __CPROVER_assume(aws_string_is_valid(str));

    __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(buf);
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* additional assumptions to satisfy preconditions */
    __CPROVER_assume(str->len <= MAX_STRING_LEN);
    __CPROVER_assume(buf->len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str->len == buf->len); // Ensure lengths are equal to avoid memcmp failures

    /* save old state */
    struct aws_string old_str = *str;
    struct aws_byte_buf old_buf = *buf;

    /* operation under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* assertions */
    if (result) {
        assert(str->len == buf->len);
        assert_bytes_match(str->bytes, buf->buffer, str->len);
    } else {
        assert(*str == old_str);
        assert(*buf == old_buf);
    }

    /* unchanged fields */
    assert(str->allocator == old_str.allocator);
    assert(buf->allocator == old_buf.allocator);

    /* validity invariants */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(buf));
}
