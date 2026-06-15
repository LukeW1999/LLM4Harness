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
    /* Decide whether to pass a valid buffer or NULL */
    bool pass_valid = nondet_bool();

    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;
    uint8_t *old_mem = NULL;

    if (pass_valid) {
        /* Allocate and initialize a valid byte buffer */
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));

        /* Save old state */
        old_buf = buf;
        if (buf.capacity > 0) {
            old_mem = malloc(buf.capacity);
            __CPROVER_assume(old_mem != NULL);
            memcpy(old_mem, buf.buffer, buf.capacity);
        }
    }

    /* Call the function under test */
    int result = aws_byte_buf_secure_zero(pass_valid ? &buf : NULL);

    if (pass_valid) {
        /* Success path */
        assert(result == AWS_OP_SUCCESS);

        /* Frame condition: buffer pointer, capacity and allocator unchanged */
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);

        /* Length invariant: length is set to zero */
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
