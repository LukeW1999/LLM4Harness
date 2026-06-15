#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to create a nondeterministic C string (or NULL) */
static char *make_nondet_c_str(void) {
    const size_t MAX_STR_LEN = 256;

    char *c_str = nondet_char_pointer();
    __CPROVER_assume(c_str != NULL || nondet_bool()); /* allow NULL */

    if (c_str != NULL) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_STR_LEN);

        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        for (size_t i = 0; i < len; ++i) {
            c_str[i] = nondet_char();
        }
        c_str[len] = '\0';
    }
    return c_str;
}

void aws_byte_buf_from_c_str_harness(void) {
    /* Arrange: create a nondeterministic input string */
    char *c_str = make_nondet_c_str();

    /* Act: invoke the function under verification */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Assert: allocator must be NULL */
    assert(buf.allocator == NULL);

    /* Assert: capacity equals length */
    assert(buf.capacity == buf.len);

    if (c_str == NULL) {
        /* When input is NULL, the buffer must be empty */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        /* Length must match the string length */
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        /* Buffer must be non‑NULL and contain the same bytes as the source string */
        assert(buf.buffer != NULL);
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == (uint8_t)c_str[i]);
        }
    }
}
