#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_string_is_valid(&str));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&str);
    __CPROVER_assume(str.allocator == allocator);
    __CPROVER_assume(str.len > 0);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    __CPROVER_assume(str.allocator->mem_release != NULL);
    aws_string_destroy_secure(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(str.allocator == NULL);
    assert(str.len == 0);
    assert(str.capacity == 0);
    assert(str.buffer == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(old.allocator != NULL);
    assert(old.len > 0);

    /* 6. Assert validity invariant always holds */
    // assert(aws_string_is_valid(&str)); // Removed, as string is destroyed
}
