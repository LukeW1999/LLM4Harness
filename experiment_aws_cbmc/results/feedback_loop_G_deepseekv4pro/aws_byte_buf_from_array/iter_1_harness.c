#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src (const, must not change) */
    struct aws_byte_buf old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must remain unchanged and valid */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* dest must be valid after call */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* dest->allocator must be the passed allocator */
        assert(dest.allocator == allocator);

        if (old_src.buffer == NULL) {
            /* src was empty, dest should be zeroed but with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest should have same len and capacity as src */
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            /* dest->buffer must be allocated and contain the same data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* result == AWS_OP_ERR: dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
