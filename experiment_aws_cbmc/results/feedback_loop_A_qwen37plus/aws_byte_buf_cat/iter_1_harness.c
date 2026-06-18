#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_byte_buf old_dest = dest;

    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    struct aws_byte_buf buf2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    int result = aws_byte_buf_cat(&dest, 2, &buf1, &buf2);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_dest.len + buf1.len + buf2.len);
    } else {
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + buf1.len + buf2.len);
    }

    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);
    assert(dest.allocator == old_dest.allocator);

    assert(aws_byte_buf_is_valid(&dest));
}
