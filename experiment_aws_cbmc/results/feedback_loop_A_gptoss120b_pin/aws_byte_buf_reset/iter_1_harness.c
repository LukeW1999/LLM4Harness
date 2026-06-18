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
    /* allocator used by the function */
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer – must be valid */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer – output of the function */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest may contain any initial garbage; no need to allocate its buffer */

    /* Save old states for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* The destination must always be a valid byte buffer after the call */
    assert(aws_byte_buf_is_valid(&dest));

    /* The source buffer must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* Allocator of the destination must be the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* Length and capacity must match the source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of the new buffer must equal the source contents */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure the destination is zeroed and allocator cleared */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Fields of dest that are not mentioned as changing are already covered
       by the above assertions (buffer, len, capacity, allocator). */
}
