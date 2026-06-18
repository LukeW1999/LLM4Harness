#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stddef.h>

/* Maximum size for the buffer used in the harness */
#define MAX_BUFFER_SIZE 64

/* CBMC nondeterministic size_t */
size_t nondet_size_t(void);

void aws_byte_buf_advance_harness(void) {
    /* 1. Allocate a concrete backing array */
    unsigned char backing[MAX_BUFFER_SIZE];

    /* 2. Declare and initialize the source buffer */
    struct aws_byte_buf buffer = {0};
    buffer.buffer = backing;
    buffer.capacity = MAX_BUFFER_SIZE;
    buffer.allocator = aws_default_allocator();
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);
    __CPROVER_assume(buffer.len > 0);               /* non‑empty source */

    /* 3. Choose a nondeterministic advance length that satisfies the precondition */
    size_t len = nondet_size_t();
    __CPROVER_assume(len > 0);
    __CPROVER_assume(len <= buffer.len);

    /* 4. Save old state for later checks */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Declare the output buffer (will be filled by the function) */
    struct aws_byte_buf output = {0};

    /* 6. Call the function under test */
    int result = aws_byte_buf_advance(&buffer, &output, len);

    /* 7. Verify post‑conditions */
    assert(result == 0);
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.allocator == old_buffer.allocator);

    assert(output.buffer == old_buffer.buffer);
    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.allocator == old_buffer.allocator);

    /* 8. Preserve invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
