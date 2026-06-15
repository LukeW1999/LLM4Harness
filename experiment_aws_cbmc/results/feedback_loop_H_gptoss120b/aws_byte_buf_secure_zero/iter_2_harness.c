#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* nondeterministically decide whether the buffer is NULL */
    if (nondet_bool()) {
        buf.buffer   = NULL;
        buf.capacity = 0;
        buf.len      = 0;
    } else {
        /* keep the allocated buffer, set len nondeterministically within capacity */
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);

        /* fill the allocated memory with nondeterministic data */
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state for immutability checks */
    struct aws_byte_buf old = buf;

    /* call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* post‑condition checks */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    if (buf.buffer != NULL) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
