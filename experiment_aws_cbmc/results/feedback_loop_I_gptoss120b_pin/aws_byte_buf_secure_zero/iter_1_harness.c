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
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Bound and allocate src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state of src (dest is uninitialized, its old state is irrelevant) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity must match src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* buffer pointer and contents */
        if (src.capacity != 0) {
            assert(dest.buffer != NULL);
            /* the first src.len bytes must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* zero capacity implies null buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* dest must still satisfy the validity predicate */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* 5. Fields of src must remain unchanged regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariant for src */
    assert(aws_byte_buf_is_valid(&src));
}
