#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    // 1. Declare and bound data structures
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    // 2. Save old state BEFORE calling
    struct aws_byte_buf old_dest = dest;

    // 3. Call function under test
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    // 4. Assert postconditions for BOTH success and failure paths
    if (result == AWS_OP_SUCCESS) {
        // "Dest capacity and len will be equal to the src len."
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        
        // "Allocator of the dest will be identical with parameter allocator."
        assert(dest.allocator == allocator);
        
        // "If src buffer is null the dest will have a null buffer with a len and a capacity of 0"
        if (src.len == 0) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            // "A new memory zone is allocated for dest->buffer."
            assert(dest.buffer != NULL);
            // The contents should match the source
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
        
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        // "Returns AWS_OP_ERR when memory can't be allocated."
        // The implementation calls AWS_ZERO_STRUCT(*dest) at the start, so it remains zeroed.
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
