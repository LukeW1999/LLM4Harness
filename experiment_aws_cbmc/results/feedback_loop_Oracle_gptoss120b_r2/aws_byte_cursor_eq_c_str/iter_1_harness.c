#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_eq_c_str_harness(void) {
    /* nondet cursor */
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_BUFFER_SIZE);

    if (cur.len > 0) {
        cur.ptr = malloc(cur.len);
        __CPROVER_assume(cur.ptr != NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < cur.len; ++i) {
            cur.ptr[i] = nondet_uint8_t();
        }
    } else {
        cur.ptr = NULL;
    }

    /* nondet c_str */
    char *c_str = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_len] = '\0';

    /* ground‑truth preconditions */
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* save old state for frame condition checks */
    size_t old_len = cur.len;
    uint8_t *old_ptr = cur.ptr;
    char *c_str_copy = malloc(c_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_len + 1);

    /* call function under test */
    bool rv = aws_byte_cursor_eq_c_str(&cur, c_str);

    /* compute expected result using the specification of aws_array_eq_c_str */
    size_t c_str_len = strlen(c_str);
    bool expected;
    if (cur.len == c_str_len) {
        if (cur.len == 0) {
            expected = true;
        } else {
            expected = (memcmp(cur.ptr, c_str, cur.len) == 0);
        }
    } else {
        expected = false;
    }

    /* post‑condition 1: return value matches specification */
    assert(rv == expected);

    /* post‑condition 2: cursor unchanged (frame condition) */
    assert(cur.len == old_len);
    assert(cur.ptr == old_ptr);
    if (cur.len > 0) {
        assert(memcmp(cur.ptr, old_ptr, cur.len) == 0);
    }

    /* post‑condition 3: c_str unchanged */
    assert(strcmp(c_str, c_str_copy) == 0);

    /* clean up */
    if (cur.ptr) free(cur.ptr);
    free(c_str);
    free(c_str_copy);
    return 0;
}
