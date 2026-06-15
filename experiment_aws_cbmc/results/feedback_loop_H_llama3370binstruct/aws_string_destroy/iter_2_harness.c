#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&str);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    aws_string_destroy(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(str.allocator == NULL);
    assert(str.buffer == NULL);
    assert(str.len == 0);
    assert(str.capacity == 0);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.len == old.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str) == false);
}

int main() {
    aws_string_destroy_harness();
    return 0;
}
