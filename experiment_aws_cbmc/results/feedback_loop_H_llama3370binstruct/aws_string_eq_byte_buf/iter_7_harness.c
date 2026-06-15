#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string str;
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_string old_str = str;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_string_eq_byte_buf(&str, &buf);

    if (result) {
        assert(str.len == old_str.len);
        assert(str.allocator == old_str.allocator);
        assert(buf.len == old_buf.len);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
    } else {
        assert(str.len == old_str.len);
        assert(str.allocator == old_str.allocator);
        assert(buf.len == old_buf.len);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
    }

    assert(aws_string_is_valid(&str));
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}

int main() {
    aws_string_eq_byte_buf_harness();
    return 0;
}
