#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    const char *c_str = (const char *)nondet_uint8_t();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old_str = str;
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(&str, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(aws_string_eq(&str, (struct aws_string *)c_str));
    } else {
        assert(!aws_string_eq(&str, (struct aws_string *)c_str));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));
}
