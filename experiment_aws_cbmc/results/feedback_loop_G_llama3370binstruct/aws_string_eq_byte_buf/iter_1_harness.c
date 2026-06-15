#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(&str, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert_bytes_match(str.bytes, buf.buffer, str.len);
    } else {
        assert(str.len != buf.len || !aws_array_eq(str.bytes, str.len, buf.buffer, buf.len));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
