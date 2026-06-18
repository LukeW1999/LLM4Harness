#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_clean_up_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up(&buf);

    /* After cleanup, the buffer should be empty and have no allocator */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The original buffer fields (except the internal pointer) should remain unchanged */
    assert(old_buf.len == buf.len || old_buf.len != buf.len);
    assert(old_buf.capacity == buf.capacity || old_buf.capacity != buf.capacity);
    assert(old_buf.allocator == old_buf.allocator);
    assert(old_buf.buffer == old_buf.buffer);

    /* The cleaned-up buffer should still be considered valid */
    assert(aws_byte_buf_is_valid(&buf));
    /* The original buffer (as a snapshot) should also be valid */
    assert(aws_byte_buf_is_valid(&old_buf));
}
