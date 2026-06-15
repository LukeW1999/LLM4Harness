#include <stddef.h>
#include <stdlib.h>
#include "aws/common/byte_buf.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* allocator can be NULL or a nondeterministic pointer */
    allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* When source buffer is NULL, dest should be zeroed and allocator set */
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == allocator);
        } else {
            /* When source buffer is non‑NULL, dest should be a copy */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
            assert(dest.allocator == allocator);
        }
    } else {
        /* Allocation failure – dest is zeroed, including allocator */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
