#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically choose a C string (may be NULL) */
    char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate len+1 bytes for the string plus terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill the string with nondeterministic bytes and terminate */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = nondet_uint8_t();
        }
        c_str[len] = '\0';
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Post‑condition assertions */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        /* strlen is bounded by the allocation size, so CBMC can evaluate it */
        assert(cur.len == strlen(c_str));
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_cursor_is_valid(&cur));
}
