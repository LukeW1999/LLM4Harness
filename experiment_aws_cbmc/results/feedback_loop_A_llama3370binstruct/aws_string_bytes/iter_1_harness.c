#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = str;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(&str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(result == str.bytes);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old.allocator);
    assert(str.len == old.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
}
