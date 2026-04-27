#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_eq_byte_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_byte_buf buf;
    size_t max_str_len = nondet_size_t();
    size_t max_buf_len = nondet_size_t();
    __CPROVER_assume(max_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(max_buf_len <= MAX_BUFFER_SIZE);
    struct aws_allocator *allocator = aws_default_allocator();
    str.allocator = allocator;
    str.len = max_str_len;
    buf.allocator = allocator;
    buf.len = max_buf_len;
    buf.capacity = max_buf_len;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(AWS_MEM_IS_READABLE(str.bytes, str.len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.len));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(&str, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (str.len == 0 && buf.len == 0) {
        assert(result == true);
    } else if (str.len != buf.len) {
        assert(result == false);
    } else {
        assert(result == aws_array_eq(str.bytes, str.len, buf.buffer, buf.len));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_string_is_valid(&str));
}
