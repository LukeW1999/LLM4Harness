#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buf;
    struct aws_byte_buf output;
    size_t len;

    /* Nondeterministic initialization */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Save pre-call state */
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t *old_buf_buffer = buf.buffer;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    bool result = aws_byte_buf_advance(&buf, &output, len);

    /* Postcondition 1: Return value correctness */
    if (old_buf_capacity - old_buf_len >= len) {
        /* Should succeed */
        assert(result == true);

        /* Postcondition 2: Output buffer invariants on success */
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);

        /* Output buffer points to the correct location in the original buffer */
        if (len == 0) {
            /* When len is 0, output.buffer may be NULL or old_buf_buffer + old_buf_len */
            assert(output.buffer == NULL || output.buffer == old_buf_buffer + old_buf_len);
        } else {
            assert(output.buffer == old_buf_buffer + old_buf_len);
        }

        /* Postcondition 3: Input buffer len is updated correctly */
        assert(buf.len == old_buf_len + len);

        /* Postcondition 4: Input buffer capacity and buffer pointer unchanged */
        assert(buf.capacity == old_buf_capacity);
        assert(buf.buffer == old_buf_buffer);
        assert(buf.allocator == old_buf_allocator);

        /* Postcondition 5: Output is a valid byte buf */
        assert(aws_byte_buf_is_valid(&output));

        /* Postcondition 6: Input buffer remains valid */
        assert(aws_byte_buf_is_valid(&buf));

    } else {
        /* Should fail */
        assert(result == false);

        /* Postcondition 7: Output is zeroed on failure */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Postcondition 8: Input buffer is unchanged on failure */
        assert(buf.len == old_buf_len);
        assert(buf.capacity == old_buf_capacity);
        assert(buf.buffer == old_buf_buffer);
        assert(buf.allocator == old_buf_allocator);

        /* Postcondition 9: Input buffer remains valid */
        assert(aws_byte_buf_is_valid(&buf));
    }
}
