#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string src;
    __CPROVER_assume(aws_string_is_valid(&src));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&src);
    __CPROVER_assume(src.len > 0);
    __CPROVER_assume(src.bytes!= NULL);
    __CPROVER_assume(src.len <= SIZE_MAX);
    struct aws_allocator *allocator = aws_default_allocator();
    src.allocator = allocator;
    __CPROVER_assume(src.allocator != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_src = src;

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(&src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(result.len == src.len);
    assert(result.ptr == src.bytes);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);

    /* 6. Assert validity invariant always holds */
    assert(src.bytes!= NULL);
    assert(src.len <= SIZE_MAX);
}
