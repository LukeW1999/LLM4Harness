#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string */
    size_t max_len = MAX_BUFFER_SIZE;
    __CPROVER_assume(max_len > 0);
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    char *c_str = malloc(len + 1);
    if (c_str) {
        /* fill the string with nondet bytes (no interior NUL) */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[len] = '\0';
    } else {
        /* allow NULL pointer */
        c_str = NULL;
    }

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Post‑condition checks */
    if (c_str) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == len);
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* 4. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
