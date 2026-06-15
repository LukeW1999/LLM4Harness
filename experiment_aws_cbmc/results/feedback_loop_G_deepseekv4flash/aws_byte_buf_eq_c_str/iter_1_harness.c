#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_eq_c_str_harness() {
    /* nondet inputs */
    struct aws_byte_buf buf;
    char *c_str;

    /* assume valid buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* assume c_str is a valid null‑terminated string (bounded length) */
    size_t str_len;
    __CPROVER_assume(str_len < MAX_STRING_LEN);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str_len + 1));
    __CPROVER_assume(c_str[str_len] == '\0');

    /* save old state for unchanged checks */
    struct aws_byte_buf old_buf = buf;
    char *old_c_str = c_str;

    /* call the function */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* assertion: buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* assertion: all fields of buf remain unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* assertion: c_str pointer is unchanged */
    assert(c_str == old_c_str);

    /* assertion: result is a boolean value (no symbolic leak) */
    assert(result == true || result == false);

    /* memory safety: CBMC implicitly checks all memory accesses */
}
