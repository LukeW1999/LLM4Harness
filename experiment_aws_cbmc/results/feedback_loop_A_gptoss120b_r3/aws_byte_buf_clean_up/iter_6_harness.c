#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);

    struct aws_allocator *alloc = aws_default_allocator();
    buf.allocator = alloc;

    if (nondet_bool()) {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
        buf.allocator = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up(&buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_valid(&old_buf));
}
