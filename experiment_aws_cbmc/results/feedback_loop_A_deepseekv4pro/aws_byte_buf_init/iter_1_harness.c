#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_harness() {
    /* 1. Set up a valid, bounded byte_buf (old state) */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    /* 2. Choose parameters nondeterministically */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Call the function under test */
    int rv = aws_byte_buf_init(&buf, allocator, capacity);

    /* 4. Validity invariant must hold after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* 5. Postconditions for both success and failure paths */
    if (rv == AWS_OP_SUCCESS) {
        /* Success: fields set as documented */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);

        /* Buffer is NULL only if capacity is zero */
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
        }
    } else {
        assert(rv == AWS_OP_ERR);
        /* Failure: allocation failed (capacity > 0), struct completely zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }
}
