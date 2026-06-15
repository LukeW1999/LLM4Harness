#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf buf1;
    struct aws_byte_buf buf2;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    bool result = aws_byte_buf_eq(&buf1, &buf2);

    if (buf1.len == buf2.len) {
        if (result) {
            assert_bytes_match(buf1.buffer, buf2.buffer, buf1.len);
        } else {
            assert(!bytes_match(buf1.buffer, buf2.buffer, buf1.len));
        }
    } else {
        assert(!result);
    }

    assert(aws_byte_buf_is_valid(&buf1));
    assert(aws_byte_buf_is_valid(&buf2));
}
