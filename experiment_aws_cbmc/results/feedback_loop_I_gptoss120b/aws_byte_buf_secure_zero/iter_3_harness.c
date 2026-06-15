#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_secure_zero_harness(void) {
    bool pass_valid = nondet_bool();

    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;
    struct aws_allocator *old_allocator = NULL;
    uint8_t *old_buffer_ptr = NULL;
    size_t old_capacity = 0;

    if (pass_valid) {
        /* Allocate a valid buffer */
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));

        /* Save frame‑condition state */
        old_buf = buf;
        old_allocator = buf.allocator;
        old_buffer_ptr = buf.buffer;
        old_capacity = buf.capacity;
    }

    int result = aws_byte_buf_secure_zero(pass_valid ? &buf : NULL);

    if (pass_valid) {
        /* Success path */
        assert(result == AWS_OP_SUCCESS);

        /* Frame condition: allocator and buffer pointer unchanged */
        assert(buf.allocator == old_allocator);
        assert(buf.buffer == old_buffer_ptr);
        assert(buf.capacity == old_capacity);

        /* Length invariant */
        assert(buf.len == 0);

        /* Memory zeroed */
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }

        /* Validity predicate holds after the call */
        assert(aws_byte_buf_is_valid(&buf));
    } else {
        /* Failure path (NULL argument) */
        assert(result != AWS_OP_SUCCESS);
    }

    /* Allocator pointer itself never changes */
    assert(aws_default_allocator() == aws_default_allocator());
}
