#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    buf.allocator = allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    struct store_byte_from_buffer old_byte;
    if (old.buffer != NULL && old.capacity > 0) {
        save_byte_from_array(old.buffer, old.capacity, &old_byte);
    }

    aws_byte_buf_clean_up_secure(&buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == old.allocator);

    if (old.buffer != NULL && old.capacity > 0) {
        assert(old.buffer[old_byte.idx] == 0);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
