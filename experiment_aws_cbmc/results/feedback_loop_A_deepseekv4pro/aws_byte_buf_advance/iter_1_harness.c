#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness() {
    /* buffer */
    struct aws_byte_buf buf;
    /* bound and allocate buffer member */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old_buf = buf;

    /* output struct – will be overwritten entirely */
    struct aws_byte_buf out;
    out.len      = 0;
    out.buffer   = NULL;
    out.capacity = 0;
    out.allocator = NULL;

    /* len can be any value */
    size_t len = nondet_size_t();

    /* call the function under test */
    bool result = aws_byte_buf_advance(&buf, &out, len);

    if (result) {
        /* success path */
        assert(buf.len == old_buf.len + len);
        /* output fields as per documentation */
        assert(out.capacity == len);
        assert(out.len == 0);
        assert(out.allocator == NULL);
        /* out.buffer must point exactly into the old buffer at old_buf.len offset */
        if (old_buf.buffer == NULL) {
            assert(out.buffer == NULL);
        } else {
            assert(out.buffer == old_buf.buffer + old_buf.len);
        }
        /* buffer fields that must remain unchanged */
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
    } else {
        /* failure path */
        /* buffer must be completely unchanged */
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        /* output must be zeroed */
        assert(out.len == 0);
        assert(out.buffer == NULL);
        assert(out.capacity == 0);
        assert(out.allocator == NULL);
    }

    /* validity invariants hold for both buffers */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_valid(&out));
}
