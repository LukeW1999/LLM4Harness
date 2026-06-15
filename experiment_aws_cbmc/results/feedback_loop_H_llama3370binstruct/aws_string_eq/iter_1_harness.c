#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string a;
    struct aws_string b;
    __CPROVER_assume(aws_string_is_valid(&a));
    __CPROVER_assume(aws_string_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_a = a;
    struct aws_string old_b = b;

    /* 3. Call function under test */
    bool result = aws_string_eq(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == true) {
        assert(a.len == b.len);
        assert(aws_array_eq(a.bytes, a.len, b.bytes, b.len));
    } else {
        assert(a.len != b.len || !aws_array_eq(a.bytes, a.len, b.bytes, b.len));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a.allocator == old_a.allocator);
    assert(a.len == old_a.len);
    assert(b.allocator == old_b.allocator);
    assert(b.len == old_b.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&a));
    assert(aws_string_is_valid(&b));
}
