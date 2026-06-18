#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Define a reasonable bound for the cursor buffer size */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_cursor_eq_c_str_harness(void) {
    /* --- Setup cursor --- */
    struct aws_byte_cursor cur;
    /* Allocate a nondeterministic buffer bounded by MAX_BUFFER_SIZE */
    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= MAX_BUFFER_SIZE);
    cur.ptr = malloc(buf_len);
    __CPROVER_assume(cur.ptr != NULL);
    /* Fill the buffer with nondeterministic data */
    __CPROVER_assume(__CPROVER_is_fresh(cur.ptr, buf_len));
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= buf_len);

    /* --- Setup C string --- */
    /* Allocate a nondeterministic null‑terminated string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE); /* keep it reasonable */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Fill with nondet bytes and terminate */
    __CPROVER_assume(__CPROVER_is_fresh(c_str, c_str_len + 1));
    c_str[c_str_len] = '\0';

    /* --- Apply ground‑truth preconditions --- */
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* --- Preserve old state for frame condition checks --- */
    struct aws_byte_cursor cur_old = cur;
    uint8_t *buf_old = malloc(cur.len);
    __CPROVER_assume(buf_old != NULL);
    memcpy(buf_old, cur.ptr, cur.len);

    char *c_str_old = malloc(strlen(c_str) + 1);
    __CPROVER_assume(c_str_old != NULL);
    strcpy(c_str_old, c_str);

    /* --- Call the function under test --- */
    bool rv = aws_byte_cursor_eq_c_str(&cur, c_str);

    /* --- Compute expected result using the specification of aws_array_eq_c_str --- */
    size_t c_len = strlen(c_str);
    bool expected = false;
    if (cur.len == c_len) {
        expected = (memcmp(cur.ptr, c_str, cur.len) == 0);
    }

    /* --- Post‑condition assertions --- */
    /* 1. Return value correctness */
    assert(rv == expected);

    /* 2. Frame conditions: inputs must be unchanged */
    assert(cur.ptr == cur_old.ptr);
    assert(cur.len == cur_old.len);
    assert(memcmp(cur.ptr, buf_old, cur.len) == 0);
    assert(strcmp(c_str, c_str_old) == 0);

    /* Clean up */
    free(buf_old);
    free(c_str_old);
    free(cur.ptr);
    free(c_str);

    return 0;
}
