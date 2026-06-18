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
    /* 1. nondet input string, possibly NULL */
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t len = 0;

    if (!is_null) {
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* fill with nondet non‑zero bytes */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
            __CPROVER_assume(c_str[i] != '\0');
        }
        c_str[len] = '\0';
    }

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 3. Post‑conditions */
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == len);
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* 4. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
