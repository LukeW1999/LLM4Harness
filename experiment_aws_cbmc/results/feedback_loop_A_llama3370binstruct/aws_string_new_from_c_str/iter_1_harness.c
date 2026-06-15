#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = &aws_default_allocator();
    const char *c_str = (const char *)nondet_uint8_t();
    size_t c_str_len = strlen(c_str);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_str_len);
        assert(memcmp(aws_string_bytes(result), c_str, c_str_len) == 0);
    } else {
        assert(c_str == old_c_str);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(c_str == old_c_str);

    /* 6. Assert validity invariant always holds */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
    }
}
