#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <aws/common/common.h>
#include <aws/common/byte_order.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* Symbolic inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    /* Allocate allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize buffer with nondeterministic but valid state */
    ensure_byte_buf_is_valid(&buffer);
    buffer.allocator = allocator;

    /* Preconditions for aws_byte_buf_advance */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(buffer.capacity >= len);                     /* writable up to len bytes */
    __CPROVER_assume(!(len > 0 && buffer.buffer == NULL));        /* non‑null pointer when len > 0 */

    /* Initialize output as a valid empty byte buffer */
    output.buffer = NULL;
    output.len = 0;
    output.capacity = 0;
    output.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Snapshot of input state */
    size_t old_len = buffer.len;
    size_t old_capacity = buffer.capacity;
    uint8_t *old_buffer_ptr = buffer.buffer;

    /* Call the function under verification */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Post‑condition assertions */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(buffer.allocator == allocator);
    assert(buffer.capacity == old_capacity);

    if (result) {
        /* Successful advance */
        assert(buffer.buffer == old_buffer_ptr + len);
        assert(buffer.len == old_len - len);

        assert(output.buffer == old_buffer_ptr);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == allocator);
    } else {
        /* Failure – buffer unchanged and len was insufficient */
        assert(buffer.buffer == old_buffer_ptr);
        assert(buffer.len == old_len);
        assert(len > old_len);
    }
}
