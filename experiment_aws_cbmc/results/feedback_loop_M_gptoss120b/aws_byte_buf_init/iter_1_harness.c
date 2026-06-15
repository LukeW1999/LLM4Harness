#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Choose a bounded capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* 3. Use the default allocator (non‑null) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Save old state */
    struct aws_byte_buf old = buf;

    /* 5. Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: fields are set according to the specification */
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        /* Failure: the buffer is zeroed */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 7. Invariant: the buffer must always be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
