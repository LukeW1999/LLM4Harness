#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    const char *c_str;

    /* Bound and initialize buf */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Bound and initialize c_str as a null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    c_str = (const char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    __CPROVER_assume(c_str[c_str_len] == 0); /* ensure null termination */

    /* Save old state for immutability checks */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer saved;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &saved);
    }

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Assert that buf is unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    if (buf.buffer != NULL && buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &saved);
    }

    /* Assert validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Note: result is a bool; the exact value depends on the content of the
       buffer and string. We rely on the implementation of aws_array_eq_c_str
       to correctly compare them. */
}
