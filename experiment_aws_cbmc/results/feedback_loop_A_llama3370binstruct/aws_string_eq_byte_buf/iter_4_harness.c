#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    struct aws_string str;
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_string_is_valid(&str));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    struct aws_string old_str = str;
    struct aws_byte_buf old_buf = buf;

    bool result = aws_string_eq_byte_buf(&str, &buf);

    assert(aws_string_is_valid(&str));
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    assert(str.len == old_str.len);
    assert(str.allocator == old_str.allocator);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (result) {
        assert(str.len == buf.len);
        assert(memcmp(str.buffer, buf.buffer, str.len) == 0);
    }
}
