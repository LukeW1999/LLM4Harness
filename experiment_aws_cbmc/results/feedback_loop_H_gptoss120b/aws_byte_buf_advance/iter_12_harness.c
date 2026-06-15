#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    /* 2. Use a well‑behaved allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    buffer.allocator = alloc;

    /* 3. Choose a non‑zero capacity within reasonable bounds */
    buffer.capacity = nondet_size_t();
    __CPROVER_assume(buffer.capacity > 0);
    __CPROVER_assume(buffer.capacity <= MAX_BUFFER_SIZE);

    /* 4. Allocate the underlying memory and assume success */
    buffer.buffer = (uint8_t *)aws_mem_acquire(buffer.allocator, buffer.capacity);
    __CPROVER_assume(buffer.buffer != NULL);

    /* 5. Choose a length that fits the capacity (may be zero) */
    buffer.len = nondet_size_t();
    __CPROVER_assume(buffer.len <= buffer.capacity);

    /* 6. Ensure the source buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 7. Choose an advance length that does not exceed the current length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= buffer.len);

    /* 8. Save old state for later comparison */
    struct aws_byte_buf old_buffer = buffer;

    /* 9. Initialise output (the function will populate it) */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = NULL;

    /* 10. Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 11. The function must succeed when len <= old_buffer.len */
    __CPROVER_assert(result, "aws_byte_buf_advance must succeed with len <= buffer.len");

    /* 12. Post‑conditions for the successful case */
    assert(buffer.len == old_buffer.len - len);
    assert(buffer.capacity == old_buffer.capacity);               /* capacity unchanged */
    assert(buffer.buffer == old_buffer.buffer + len);
    assert(buffer.allocator == old_buffer.allocator);

    assert(output.len == len);
    assert(output.capacity == len);
    assert(output.buffer == old_buffer.buffer);
    assert(output.allocator == old_buffer.allocator);

    /* 13. Invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
