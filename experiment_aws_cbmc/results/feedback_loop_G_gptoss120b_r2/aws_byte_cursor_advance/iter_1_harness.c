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
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_byte);
    }

    /* Destination buffer (output) – uninitialized */
    struct aws_byte_buf dest;

    /* Use the default allocator as required */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 3. Global invariants after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* 4. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.capacity > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* 5. Post‑conditions depending on the return value */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src had no buffer – dest should be a zeroed buffer with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* Successful allocation – dest mirrors src (except allocator) and has its own buffer */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failed – dest is zeroed and has no allocator */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
