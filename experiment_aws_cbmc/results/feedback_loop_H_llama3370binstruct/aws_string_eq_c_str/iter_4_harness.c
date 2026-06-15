#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
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
        struct aws_string c_str_str = aws_string_from_c_str(aws_default_allocator(), c_str);
        assert(aws_string_eq(&str, &c_str_str));
        aws_string_destroy(&c_str_str);
    } else {
        struct aws_string c_str_str = aws_string_from_c_str(aws_default_allocator(), c_str);
        assert(!aws_string_eq(&str, &c_str_str));
        aws_string_destroy(&c_str_str);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);

    /* 6. Assert validity invariant always holds */
    assert(aws_string_is_valid(&str));

    /* 7. Test with NULL c_str */
    result = aws_string_eq_c_str(&str, NULL);
    assert(!result);

    /* 8. Test with empty c_str */
    const char *empty_c_str = "";
    result = aws_string_eq_c_str(&str, empty_c_str);
    if (str.len == 0 && str.data[0] == '\0') {
        assert(result);
    } else {
        assert(!result);
    }
}
