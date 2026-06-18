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

    bool src_has_buf = nondet_bool();

    if (src_has_buf) {
        /* nondet capacity and length within bounds */
        src.capacity = nondet_size_t();
        __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
        src.len = nondet_size_t();
        __CPROVER_assume(src.len <= src.capacity);

        src.allocator = aws_default_allocator();

        /* allocate the underlying buffer */
        ensure_byte_buf_has_allocated_buffer_member(&src);
    } else {
        /* empty buffer case */
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
        src.allocator = aws_default_allocator();
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old source state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* 2. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 3. Call function under test */
    struct aws_allocator *alloc = aws_default_allocator();
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity must mirror the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* source empty → destination empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* destination must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* copied contents must match for the length of the source */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Source must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
