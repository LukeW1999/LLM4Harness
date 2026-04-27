#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* parameters */
    bool zero_contents = nondet_bool();

    /* bound and allocate buf */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old = buf;

    /* call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* assert postconditions */
    if (zero_contents) {
        /* Check that the buffer has been zeroed */
        for (size_t i = 0; i < old.len; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* len is reset to 0 */
    assert(buf.len == 0);

    /* allocator and capacity are unchanged */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);

    /* buffer pointer is unchanged */
    assert(buf.buffer == old.buffer);

    /* assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
