#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    ensure_byte_buf_has_allocated_buffer_member(&buf);

    if (nondet_bool()) {
        buf.allocator = aws_default_allocator();
    } else {
        buf.allocator = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);

    assert(aws_byte_buf_is_valid(&buf));
}
