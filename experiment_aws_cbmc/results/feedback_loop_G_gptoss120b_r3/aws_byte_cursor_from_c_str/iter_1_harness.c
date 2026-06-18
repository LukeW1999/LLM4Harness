#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non-deterministic input string (may be NULL) */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = nondet_size_t();
        __CPROVER_assume(max_len <= MAX_BUFFER_SIZE);
        /* allocate space for a string of length max_len plus null terminator */
        char *buf = malloc(max_len + 1);
        __CPROVER_assume(buf != NULL);
        /* make the buffer readable/writable; contents are nondet */
        /* ensure null termination */
        buf[max_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state of input (pointer value) */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Postconditions */
    /* pointer must be the same as the input */
    assert(cur.ptr == (uint8_t *)c_str);

    if (c_str) {
        size_t expected_len = strlen(c_str);
        assert(cur.len == expected_len);
    } else {
        assert(cur.len == 0);
    }

    /* 5. Input must remain unchanged */
    assert(c_str == old_c_str);

    /* 6. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
