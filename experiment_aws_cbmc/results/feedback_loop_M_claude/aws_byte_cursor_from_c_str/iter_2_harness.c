#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Create a non-deterministic c_str input */
    /* We need a bounded null-terminated string */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Either pass NULL or a valid null-terminated string */
    bool use_null = nondet_bool();
    const char *c_str;

    if (use_null) {
        c_str = NULL;
    } else {
        /* Allocate a buffer of str_len + 1 bytes (for null terminator) */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Fill buffer with non-null bytes up to str_len, then null terminate */
        for (size_t i = 0; i < str_len; i++) {
            buf[i] = nondet_uint8_t();
            __CPROVER_assume(buf[i] != '\0');
        }
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 3. Assert postconditions */

    if (c_str == NULL) {
        /* When c_str is NULL, ptr should be NULL and len should be 0 */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* When c_str is non-NULL:
         * - ptr should point to the same memory as c_str
         * - len should be strlen(c_str)
         * - since we ensured no null bytes before str_len, strlen == str_len
         */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
        assert(result.len == str_len);
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
