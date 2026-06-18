#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    /* Buffer under test */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;
    uint8_t old_contents[MAX_BUFFER_SIZE];
    if (buf.buffer != NULL) {
        memcpy(old_contents, buf.buffer, buf.capacity);
    }

    /* Nondeterministic zero flag */
    bool zero = nondet_bool();

    /* Call the function under test */
    aws_byte_buf_reset(&buf, zero);

    /* Post‑conditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    if (zero && buf.buffer != NULL) {
        /* Buffer must be zeroed */
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        /* Buffer contents must be unchanged */
        if (buf.buffer != NULL) {
            assert_bytes_match(buf.buffer, old_contents, old.capacity);
        }
    }
}
