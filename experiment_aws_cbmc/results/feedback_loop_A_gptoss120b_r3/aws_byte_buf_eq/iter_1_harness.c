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
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Save a byte from src for immutability check (if src has data) */
    struct store_byte_from_buffer src_store;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_store);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks */

    /* validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    /* src must be unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.capacity == old_src.capacity);
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_store);
    }

    if (result == AWS_OP_SUCCESS) {
        /* allocator of dest is the one passed in */
        assert(dest.allocator == alloc);

        /* capacity and len mirror src */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* src was “null” – dest should be zeroed */
            assert(dest.buffer == NULL);
            assert(dest.capacity == 0);
            assert(dest.len == 0);
        } else {
            /* a new buffer must have been allocated */
            assert(dest.buffer != NULL);
            /* it must be a different allocation from src */
            assert(dest.buffer != src.buffer);
            /* the copied bytes must match */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.len == 0);
        assert(dest.allocator == NULL);
    }

    /* fields of dest that are not mentioned as changing are already covered
       by the above assertions (allocator, buffer, capacity, len). */
}
