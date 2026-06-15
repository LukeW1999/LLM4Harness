#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    bool is_zeroed = nondet_bool();
    if (is_zeroed) {
        /* buffer is already zeroed */
        buf.allocator = NULL;
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
    } else {
        /* buffer is valid and has an allocated buffer */
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    }

    /* Save old allocator for additional checks */
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Postcondition: buffer is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Additional: if the original allocator was non‑null, the buffer pointer has been freed */
    if (old_allocator != NULL) {
        assert(buf.buffer == NULL);
    }
}
