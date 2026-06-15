#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Proof helper declarations */
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

/* Bounding constant – adjust as needed for the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically choose a C string (or NULL) */
    char *c_str = NULL;

    if (nondet_bool()) {
        /* Allocate a nondeterministic length string, bounded by MAX_BUFFER_SIZE */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Fill the string with nondeterministic bytes and null‑terminate it */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[len] = '\0';
    } else {
        c_str = NULL;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Post‑condition checks */
    /* The returned pointer must equal the input pointer (cast) */
    assert(cur.ptr == (uint8_t *)c_str);

    if (c_str) {
        /* Length must equal the string length (strlen) */
        size_t expected_len = strlen(c_str);
        assert(cur.len == expected_len);
    } else {
        /* NULL input yields length zero */
        assert(cur.len == 0);
    }

    /* 4. Invariant: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cur));
}
