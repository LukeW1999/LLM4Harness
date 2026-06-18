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

    /* nondeterministically decide whether src.buffer is NULL */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    } else {
        /* ensure len is within capacity */
        __CPROVER_assume(src.len <= src.capacity);
        /* keep the allocated buffer as is */
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    /* dest is uninitialized; its fields will be set by the function */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest must reflect src */
        assert(dest.allocator == alloc);
        if (src.buffer == NULL) {
            /* src empty -> dest empty */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src non‑empty -> dest allocated and copied */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must match for the length of src */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure: allocation failed, dest zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* src must be unchanged */
        assert(src.buffer == old_src.buffer);
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* dest fields other than those explicitly set are allowed to change (they are overwritten) */
    /* src must remain unchanged (already asserted in failure case, also assert here) */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
