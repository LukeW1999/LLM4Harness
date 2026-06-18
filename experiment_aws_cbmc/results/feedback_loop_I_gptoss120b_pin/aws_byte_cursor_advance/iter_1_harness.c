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

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – must be non‑NULL */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* dest – output buffer */
    struct aws_byte_buf dest;
    /* keep a copy of dest to compare on failure */
    struct aws_byte_buf old_dest = dest;

    /* src – input buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    struct aws_byte_buf old_src = src;   /* save old state for immutability checks */

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* post‑conditions */
    if (src.buffer == NULL) {
        /* src is a null buffer – function must succeed and produce an empty dest */
        assert(result == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        if (result == AWS_OP_SUCCESS) {
            /* successful allocation – dest must be a copy of src */
            assert(dest.buffer != NULL);
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);
            assert(dest.allocator == allocator);
            /* contents must match up to src.len */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            assert(aws_byte_buf_is_valid(&dest));
        } else {
            /* allocation failed – dest must be zeroed */
            assert(result == AWS_OP_ERR);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
            assert(aws_byte_buf_is_valid(&dest));
        }
    }

    /* src must remain unchanged regardless of the outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));
}
