#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    /* output will be overwritten, no need to allocate its internal buffer */

    /* nondeterministic length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output  = output;

    /* Save the bytes that may be referenced by the output on the success path */
    struct store_byte_from_buffer storage;
    if (buffer.buffer != NULL && buffer.capacity - buffer.len >= len) {
        save_byte_from_array(buffer.buffer + buffer.len, len, &storage);
    }

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions for both success and failure paths */
    if (result) {
        /* Success: buffer advanced */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Success: output describes the advanced region */
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.buffer == ((old_buffer.buffer == NULL) ? NULL : old_buffer.buffer + old_buffer.len));
        assert(output.allocator == NULL);

        /* Bytes referenced by output must match original buffer bytes */
        if (old_buffer.buffer != NULL && len > 0) {
            assert_bytes_match(old_buffer.buffer + old_buffer.len, output.buffer, len);
        }
    } else {
        /* Failure: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Failure: output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 5. Fields that must not change regardless of result (already covered above) */
    /* No additional fields to assert beyond those already checked. */

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
