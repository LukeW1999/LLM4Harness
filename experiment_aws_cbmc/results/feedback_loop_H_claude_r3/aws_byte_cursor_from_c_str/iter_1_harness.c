#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Declare input: a non-deterministic C string (may be NULL or valid) */
    const char *c_str;

    /* We non-deterministically choose whether c_str is NULL or a valid string.
     * For the non-NULL case, we need a bounded string for CBMC to handle. */
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator exists somewhere within bounds */
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 3. Assert postconditions */

    /* From the implementation:
     * cur.ptr = (uint8_t *)c_str;
     * cur.len = (cur.ptr) ? strlen(c_str) : 0;
     */

    if (c_str == NULL) {
        /* When c_str is NULL, ptr is NULL and len is 0 */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* When c_str is non-NULL, ptr points to c_str and len is strlen(c_str) */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
