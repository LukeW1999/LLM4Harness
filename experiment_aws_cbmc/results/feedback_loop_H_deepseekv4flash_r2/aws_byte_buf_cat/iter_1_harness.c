#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    /* Nondeterministic buffers */
    struct aws_byte_buf dest;
    struct aws_byte_buf buf1;
    struct aws_byte_buf buf2;

    /* Bound buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));

    /* Allocate buffer members */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    ensure_byte_buf_has_allocated_buffer_member(&buf2);

    /* Assume validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    /* Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;

    /* Total length from source buffers */
    size_t total_len = buf1.len + buf2.len;

    /* Call function with two variadic arguments */
    int result = aws_byte_buf_cat(&dest, 2, &buf1, &buf2);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest len increased by total length of all buffers */
        assert(dest.len == old_dest.len + total_len);
        /* dest capacity and allocator unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        /* dest buffer pointer unchanged */
        assert(dest.buffer == old_dest.buffer);
        /* source buffers unchanged */
        assert(buf1.len == old_buf1.len);
        assert(buf1.capacity == old_buf1.capacity);
        assert(buf1.buffer == old_buf1.buffer);
        assert(buf1.allocator == old_buf1.allocator);
        assert(buf2.len == old_buf2.len);
        assert(buf2.capacity == old_buf2.capacity);
        assert(buf2.buffer == old_buf2.buffer);
        assert(buf2.allocator == old_buf2.allocator);
    } else {
        /* On failure, dest unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* source buffers unchanged */
        assert(buf1.len == old_buf1.len);
        assert(buf1.capacity == old_buf1.capacity);
        assert(buf1.buffer == old_buf1.buffer);
        assert(buf1.allocator == old_buf1.allocator);
        assert(buf2.len == old_buf2.len);
        assert(buf2.capacity == old_buf2.capacity);
        assert(buf2.buffer == old_buf2.buffer);
        assert(buf2.allocator == old_buf2.allocator);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf1));
    assert(aws_byte_buf_is_valid(&buf2));
}
