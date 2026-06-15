#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_init_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    /* 2. Call function under test */
    int result = aws_byte_buf_init(&buf, aws_default_allocator(), nondet_size_t());

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.allocator == aws_default_allocator());
        if (buf.capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer!= NULL);
        }
    } else {
        assert(buf.len == old.len);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);
        assert(buf.buffer == old.buffer);
    }

    /* 4. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == aws_default_allocator());

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
