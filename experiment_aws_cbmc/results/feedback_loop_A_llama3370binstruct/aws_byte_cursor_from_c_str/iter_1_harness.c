#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str;
    __CPROVER_assume(c_str != NULL);
    size_t len = strlen(c_str);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == len);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(c_str == old_c_str);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
