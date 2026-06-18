#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare source byte buffer and bound its size */
    struct aws_byte_buf src;
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src.len <= src.capacity);
    __CPROVER_assume(src.allocator != NULL);

    /* nondeterministically decide whether src.buffer is NULL */
    if (nondet_bool()) {
        src.buffer = NULL;
        src.capacity = 0;
        src.len = 0;
        src.allocator = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Destination buffer (will be initialized by the function) */
    struct aws_byte_buf dest;

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state BEFORE calling */
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
            for (size_t i = 0; i < src.len; ++i) {
                assert(dest.buffer[i] == src.buffer[i]);
            }
        }
    } else {
        /* Failure: dest should be zeroed */
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

    /* 5. src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
