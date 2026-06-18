#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string */
    const char *c_str;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Either NULL or a valid C‑string of length 'len' */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* make the allocated memory readable and initialise it */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
        }
        ((uint8_t *)c_str)[len] = '\0';
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, len + 1));
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 4. Post‑conditions */
    if (c_str == NULL) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
