#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_byte_cursor_from_c_str_harness() {
    /* Decide whether to pass NULL or a valid null-terminated string */
    bool is_null = nondet_bool();
    const char *c_str;

    /* If not NULL, allocate a buffer on the stack to hold the string */
    char str[MAX_BUFFER_SIZE];

    if (is_null) {
        c_str = NULL;
    } else {
        /* Ensure the string is null-terminated within the buffer bounds.
         * strlen(str) < MAX_BUFFER_SIZE guarantees there's a '\0' before
         * the end of the allocated array. */
        __CPROVER_assume(strlen(str) < MAX_BUFFER_SIZE);
        c_str = str;
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* Postconditions – verify the contents of the returned cursor */
    if (is_null) {
        /* When c_str is NULL, ptr must be NULL and len 0 */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When c_str is non‑NULL:
         * - ptr must point to the same memory
         * - len must be the length of the string (excluding the terminator) */
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }

    /* The returned cursor must always satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
