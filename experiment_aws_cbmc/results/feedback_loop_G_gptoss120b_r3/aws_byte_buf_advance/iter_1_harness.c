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
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* bound len to avoid overflow in arithmetic */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;
    uint8_t *old_buf_ptr = buffer.buffer;
    size_t old_len = buffer.len;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions for both success and failure paths */
    if (result) {
        /* success: buffer advanced */
        assert(buffer.len == old_len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buf_ptr);
        assert(buffer.allocator == old_buffer.allocator);

        /* output initialized from array */
        assert(output.len == 0);
        assert(output.capacity == len);
        if (old_buf_ptr == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buf_ptr + old_len);
        }
        /* aws_byte_buf_from_array sets allocator to NULL */
        assert(output.allocator == NULL);
    } else {
        /* failure: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
