#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* Symbolic input: c_str can be NULL or point to a nondet null‑terminated string */
    char *c_str;
    __CPROVER_assume(c_str == NULL || c_str != NULL);

    if (c_str != NULL) {
        /* Choose a nondet length for the string (including space for the terminator) */
        size_t len;
        __CPROVER_assume(len < 256);               /* bound the length */
        /* Allocate memory for the string plus terminator */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Fill the string with nondet bytes */
        for (size_t i = 0; i < len; ++i) {
            uint8_t nondet_byte;
            __CPROVER_assume(nondet_byte <= 255);
            c_str[i] = (char)nondet_byte;
        }
        /* Null‑terminate */
        c_str[len] = '\0';
    }

    /* PRE‑CALL SNAPSHOT */
    size_t orig_len = (c_str != NULL) ? strlen(c_str) : 0;
    const char *orig_ptr = c_str;
    uint8_t *orig_contents = NULL;
    if (orig_len > 0) {
        orig_contents = malloc(orig_len);
        __CPROVER_assume(orig_contents != NULL);
        memcpy(orig_contents, c_str, orig_len);
    }

    /* CALL UNDER TEST */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* ASSERTIONS */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* When input is NULL, buffer, len and capacity must be zero */
        assert(result.buffer == NULL);
        assert(result.len == 0);
        assert(result.capacity == 0);
    } else {
        size_t expected_len = strlen(c_str);

        /* For non‑empty strings, buffer must be non‑NULL and contain the same bytes */
        if (expected_len > 0) {
            assert(result.buffer != NULL);
            assert(memcmp(result.buffer, c_str, expected_len) == 0);
        } else {
            /* For empty strings the implementation may return a NULL buffer */
            /* No additional checks on result.buffer are required */
        }

        /* length and capacity must equal the string length (excluding terminator) */
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);

        /* null terminator must remain unchanged */
        assert(c_str[expected_len] == '\0');

        /* frame condition: original contents of the string must be unchanged */
        if (orig_len > 0) {
            assert(memcmp(orig_contents, c_str, orig_len) == 0);
        }
    }
}
