#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_cursor_eq_c_str_harness(void) {
    /* Setup cursor */
    struct aws_byte_cursor cur;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* Setup c_str - nondet null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    /* Save state before call */
    size_t old_len = cur.len;
    uint8_t *old_ptr = cur.ptr;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cur, c_str);

    /* Postcondition 1: The cursor's length is not modified */
    assert(cur.len == old_len);

    /* Postcondition 2: The cursor's pointer is not modified */
    assert(cur.ptr == old_ptr);

    /* Postcondition 3: The cursor remains valid after the call */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Postcondition 4: If cursor length differs from c_str length, result must be false */
    if (cur.len != c_str_len) {
        assert(!result);
    }

    /* Postcondition 5: If result is true, then cursor length must equal c_str length */
    if (result) {
        assert(cur.len == c_str_len);
    }
}
