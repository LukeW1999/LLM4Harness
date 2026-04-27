#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str = (const char *)malloc(nondet_size_t());
    if (c_str) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        c_str[len] = '\0'; // Null-terminate the string
        for (size_t i = 0; i < len; i++) {
            c_str[i] = nondet_uint8_t();
        }
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_cur;
    old_cur.ptr = (uint8_t *)c_str;
    old_cur.len = (old_cur.ptr) ? strlen((const char *)old_cur.ptr) : 0;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    // Success path: c_str is not NULL
    if (c_str) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        // Failure path: c_str is NULL
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function only returns a struct by value

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
