#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non‑deterministic length for the string, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a possibly NULL C‑string */
    char *c_str = malloc(len + 1);
    if (c_str) {
        /* Fill with non‑deterministic bytes and terminate */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
        }
        c_str[len] = '\0';
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Post‑condition assertions */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == len);
    }

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
