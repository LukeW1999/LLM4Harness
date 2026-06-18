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
    /* allocator must be non‑NULL */
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Save a byte from src for later content comparison */
    struct store_byte_from_buffer src_bytes;
    save_byte_from_array(src.buffer, src.len, &src_bytes);

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* Global invariants must hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* allocator is set to the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity mirror the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer != NULL) {
            /* buffer must be allocated */
            assert(dest.buffer != NULL);
            /* contents must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* null source yields null destination */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }

        /* source must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
        assert_byte_from_buffer_matches(old_src.buffer, &src_bytes);
    } else {
        /* on failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* source must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }
}
