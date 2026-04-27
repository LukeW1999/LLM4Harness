#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    size_t max_size = MAX_BUFFER_SIZE;
    __CPROVER_assume(max_size > 0);
    uint8_t *buffer = malloc(max_size);
    __CPROVER_assume(buffer != NULL);
    buf.buffer = buffer;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= max_size); // Ensure len does not exceed capacity
    buf.capacity = max_size;
    buf.allocator = aws_default_allocator();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(buf.len == 0);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Free allocated memory */
    free(buffer);
}
