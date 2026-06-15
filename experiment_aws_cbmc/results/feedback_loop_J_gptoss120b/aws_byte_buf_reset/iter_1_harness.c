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

    /* nondet decide whether src is a null buffer */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
    }

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Save old state */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest reflects src */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        if (src.buffer == NULL) {
            /* src null case */
            assert(dest.buffer == NULL);
        } else {
            /* src non‑null case */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure: dest zeroed */
        assert(dest.buffer == NULL);
        assert(dest.capacity == 0);
        assert(dest.len == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result (src unchanged) */
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.len == old_src.len);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
