/*=== Contract for aws_byte_cursor_from_c_str ===*/
/*
Preconditions:
  - c_str is either NULL or points to a valid null‑terminated byte array.
  - If c_str != NULL, the array length (excluding the terminating '\0') is bounded
    by MAX_STRING_LEN (e.g., 256) to avoid overflow in strlen.
  - The memory region pointed to by c_str is readable for its entire length plus the
    terminating null byte.

Postconditions (validity):
  - The returned aws_byte_cursor cur satisfies aws_byte_cursor_is_valid(&cur).
  - cur.ptr == (uint8_t *)c_str.
  - If c_str == NULL then cur.len == 0, otherwise cur.len == strlen(c_str).

Postconditions (frame):
  - The function does not modify any memory reachable from c_str.
  - No other memory locations are modified.
*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_c_str_harness(void) {
    /* nondet pointer for c_str */
    const char *c_str = NULL;

    /* decide nondeterministically whether c_str is NULL */
    __CPROVER_assume(__CPROVER_bool __c_str_is_null);
    if (!__c_str_is_null) {
        /* allocate a buffer for the string */
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= MAX_STRING_LEN);
        /* allocate at least one byte for the terminating null */
        __CPROVER_assume(buf_len > 0);
        char *buf = malloc(buf_len);
        __CPROVER_assume(buf != NULL);

        /* fill buffer with nondet data */
        for (size_t i = 0; i < buf_len; ++i) {
            buf[i] = nondet_char();
        }

        /* ensure there is a null terminator somewhere within the buffer */
        size_t null_pos = nondet_size_t();
        __CPROVER_assume(null_pos < buf_len);
        buf[null_pos] = '\0';

        c_str = buf;
    }

    /* Preserve a copy of the original memory for frame condition */
    size_t original_len = 0;
    if (c_str != NULL) {
        original_len = strlen(c_str);
        char *original_copy = malloc(original_len + 1);
        __CPROVER_assume(original_copy != NULL);
        memcpy(original_copy, c_str, original_len + 1);
        /* keep pointer to original copy for later comparison */
        const char *orig_ptr = original_copy;

        /* call the function under test */
        struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

        /* Postcondition: validity */
        assert(aws_byte_cursor_is_valid(&cur));

        /* Postcondition: pointer equality */
        assert(cur.ptr == (uint8_t *)c_str);

        /* Postcondition: length */
        if (c_str == NULL) {
            assert(cur.len == 0);
        } else {
            assert(cur.len == original_len);
        }

        /* Frame condition: original memory unchanged */
        assert(memcmp(c_str, orig_ptr, original_len + 1) == 0);

        free((void *)orig_ptr);
    } else {
        /* c_str is NULL case */
        struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

        /* Postcondition: validity */
        assert(aws_byte_cursor_is_valid(&cur));

        /* Postcondition: pointer equality */
        assert(cur.ptr == NULL);

        /* Postcondition: length */
        assert(cur.len == 0);
    }

    return 0;
}
