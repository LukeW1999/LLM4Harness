#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
char nondet_char(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

void aws_byte_cursor_from_c_str_harness(void) {
    /* Nondeterministic C string (may be NULL) */
    const char *c_str;

    /* Decide whether the pointer is NULL */
    __CPROVER_assume(nondet_bool() ? (c_str = NULL) : 1);

    if (c_str != NULL) {
        /* Allocate a nondeterministic buffer that will hold the string */
        size_t max_len = nondet_size_t();
        __CPROVER_assume(max_len < 256);               /* bound allocation size */
        char *buf = malloc(max_len + 1);
        __CPROVER_assume(buf != NULL);

        /* Fill the buffer with nondeterministic bytes */
        for (size_t i = 0; i < max_len; ++i) {
            buf[i] = nondet_char();
        }

        /* Ensure there is at least one null terminator somewhere in the buffer */
        size_t null_pos = nondet_size_t();
        __CPROVER_assume(null_pos <= max_len);
        buf[null_pos] = '\0';

        c_str = buf;
    }

    /* Preserve a copy of the original string memory for frame condition checks */
    char *orig_copy = NULL;
    size_t orig_len = 0;
    if (c_str != NULL) {
        orig_len = strlen(c_str);
        orig_copy = malloc(orig_len + 1);
        __CPROVER_assume(orig_copy != NULL);
        memcpy(orig_copy, c_str, orig_len + 1);
    }

    /* Call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* Post‑condition 1: returned cursor must satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Post‑condition 2: length and pointer invariants */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }

    /* Post‑condition 3: frame condition – the input string memory must be unchanged */
    if (c_str != NULL) {
        assert(memcmp(c_str, orig_copy, orig_len + 1) == 0);
    }

    return 0;
}
