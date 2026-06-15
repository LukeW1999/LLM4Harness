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
    /* 1. Declare structures and nondet inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    /* 2. Bound inputs */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 3. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* 4. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 5. Post‑condition checks */
    if (result) {
        /* Success path: buffer advanced and output set */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == ((old_buffer.buffer == NULL) ? NULL : old_buffer.buffer + old_buffer.len));
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == NULL);
    } else {
        /* Failure path: buffer unchanged, output zeroed */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.len == 0);
        assert(output.allocator == NULL);
    }

    /* 6. Validity invariants must hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
