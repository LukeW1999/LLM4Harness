#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – use the default allocator as required by the library */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer – may be NULL or non‑NULL */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer – output of the function */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest does not need to be valid before the call; it will be overwritten */
    memset(&dest, 0, sizeof(dest));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* The function must always leave the structures valid */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* src must be unchanged */
    assert(src.buffer   == old_src.buffer);
    assert(src.len      == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* allocator is set to the supplied allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When src has no buffer, dest is a zero‑length, zero‑capacity buffer */
            assert(dest.buffer   == NULL);
            assert(dest.len      == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* len and capacity must match src */
            assert(dest.len      == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of the new buffer must equal src's contents */
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        }
    } else {
        /* On allocation failure the destination is zeroed */
        assert(dest.buffer   == NULL);
        assert(dest.len      == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
