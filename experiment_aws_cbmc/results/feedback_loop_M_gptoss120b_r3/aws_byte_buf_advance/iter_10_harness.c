#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stddef.h>

/* Maximum size for the buffer used in the harness */
#define MAX_BUFFER_SIZE 64

void aws_byte_buf_advance_harness() {
    /* 1. Allocate a concrete backing array */
    unsigned char backing[MAX_BUFFER_SIZE];

    /* 2. Declare and initialize the source buffer */
    struct aws_byte_buf buffer = {0};
    buffer.buffer = backing;
    buffer.capacity = MAX_BUFFER_SIZE;
    buffer.allocator = NULL;               /* static buffer – no allocator */
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);

    /* 3. Declare the output buffer (will be filled by the function) */
    struct aws_byte_buf output = {0};

    /* 4. Choose a nondeterministic advance length that satisfies the precondition */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);

    /* 5. Save old state for later checks */
    struct aws_byte_buf old_buffer = buffer;

    /* 6. Call the function under test */
    int result = aws_byte_buf_advance(&buffer, &output, len);

    /* 7. Verify post‑conditions */
    assert(result == 0);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);
    assert(buffer.buffer == old_buffer.buffer + len);

    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == NULL);

    /* 8. Preserve invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
