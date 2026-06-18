#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* allocator (always a valid allocator) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* src buffer: nondeterministic but bounded and valid */
    struct aws_byte_buf src;
    src.capacity = nondet_size_t();
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    if (src.capacity > 0) {
        /* allocate the underlying buffer */
        ensure_byte_buf_has_allocated_buffer_member(&src);
        /* make sure the buffer pointer is non‑NULL when capacity > 0 */
        __CPROVER_assume(src.buffer != NULL);
    } else {
        src.buffer = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* dest buffer: nondeterministic but bounded (its initial contents are irrelevant) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest may be any valid struct before the call – we capture its old state for later comparison */
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old states */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must remain unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    /* dest must be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* When src has no buffer, dest is zeroed but allocator is set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* Successful allocation and copy */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.allocator == allocator);
            /* Verify that the copied bytes match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failure – dest is zeroed and allocator cleared */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
