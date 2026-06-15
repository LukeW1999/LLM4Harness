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

    struct aws_byte_buf old = buf;

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.len == 0);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    assert(aws_byte_buf_is_valid(&buf));
}
