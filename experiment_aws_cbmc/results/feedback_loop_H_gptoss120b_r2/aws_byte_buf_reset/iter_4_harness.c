#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_storage);
    }

    __CPROVER_bool zero; /* nondeterministic */

    aws_byte_buf_reset(&buf, zero);

    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    if (buf.buffer != NULL && old_buf.len > 0) {
        if (zero) {
            size_t i;
            for (i = 0; i < old_buf.len; ++i) {
                assert(buf.buffer[i] == 0);
            }
        } else {
            assert_byte_from_buffer_matches(buf.buffer, &buf_storage);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
