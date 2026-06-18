#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated C string */
    /* We bound the string length to keep the proof tractable */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Non-deterministically choose whether c_str is NULL or a valid string */
    bool is_null = nondet_bool();
    const char *c_str;

    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a buffer of str_len + 1 bytes for the null-terminated string */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator is present */
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state of c_str pointer before call */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 4. Assert postconditions */

    /* The input pointer must not have changed */
    assert(c_str == old_c_str);

    if (c_str == NULL) {
        /* When c_str is NULL:
         * - ptr should be NULL
         * - len should be 0 */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* When c_str is non-NULL:
         * - ptr should point to the same memory as c_str
         * - len should equal strlen(c_str) */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
        /* len should match str_len since we set buf[str_len] = '\0' */
        assert(result.len == str_len);
    }

    /* 5. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
