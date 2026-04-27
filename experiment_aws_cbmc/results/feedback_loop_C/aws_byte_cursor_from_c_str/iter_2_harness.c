#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str = nondet_bool() ? NULL : (const char *)any_memory();
    struct aws_byte_cursor old_cur = { .ptr = NULL, .len = 0 };
    struct aws_byte_cursor cur = old_cur;

    if (c_str != NULL) {
        size_t len = strlen(c_str);
        __CPROVER_assume(len <= MAX_BUFFER_SIZE); // Ensure c_str is bounded
        __CPROVER_assume(__CPROVER_is_fresh((char *)c_str, len + 1)); // Ensure c_str is a valid string
    }

    cur = aws_byte_cursor_from_c_str(c_str);

    // Assertions for success path
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.ptr == old_cur.ptr);
        assert(cur.len == old_cur.len);
    }

    // Validity invariant
    assert(aws_byte_cursor_is_valid(&cur));
}
