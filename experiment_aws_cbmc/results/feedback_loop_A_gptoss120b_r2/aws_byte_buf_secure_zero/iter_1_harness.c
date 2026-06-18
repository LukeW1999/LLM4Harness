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
    /* Allocate destination and source structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    /* Use the default allocator as required */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Bound and initialize the source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a copy of the source for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* The destination must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    /* The source must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* On success the allocator is set to the provided allocator */
        assert(dest.allocator == alloc);

        /* Length and capacity must match the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* Buffer allocation behavior */
        if (src.capacity > 0) {
            assert(dest.buffer != NULL);
            /* The contents up to src.len must be identical */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
