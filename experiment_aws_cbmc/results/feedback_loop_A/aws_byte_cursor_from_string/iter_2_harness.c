#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string src;
    src.allocator = aws_default_allocator();
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    src.bytes = bounded_malloc(src.len + 1);  // Allocate buffer for string and null terminator
    __CPROVER_assume(src.bytes != NULL);
    ensure_memory_is_initialized(src.bytes, src.len);
    src.bytes[src.len] = '\0';  // Ensure null-termination

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = src;

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(&src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(cursor.ptr == src.bytes);
    assert(cursor.len == src.len);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(src.allocator == old.allocator);
    assert(src.len == old.len);
    assert(AWS_MEM_IS_READABLE(src.bytes, src.len));

    /* 6. Assert validity invariants always holds */
    assert(aws_string_is_valid(&src));
    assert(aws_byte_cursor_is_valid(&cursor));
}
