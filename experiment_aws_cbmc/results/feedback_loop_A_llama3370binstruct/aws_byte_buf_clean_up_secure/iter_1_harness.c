#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == 0);
    assert(buf.buffer == old_buf.buffer);

    assert(aws_byte_buf_is_valid(&buf));
}
