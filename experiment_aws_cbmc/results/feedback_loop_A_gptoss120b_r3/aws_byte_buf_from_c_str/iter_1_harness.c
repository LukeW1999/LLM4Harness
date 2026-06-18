#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    const char *c_str;
    size_t max_len = MAX_BUFFER_SIZE;               /* bound for the string length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    if (nondet_bool()) {
        /* c_str is non‑NULL: allocate a buffer of length len + 1 and terminate it */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[len] = '\0';
        c_str = tmp;
    } else {
        /* c_str is NULL */
        c_str = NULL;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */

    /* allocator is always set to NULL */
    assert(buf.allocator == NULL);

    /* capacity must equal length */
    assert(buf.capacity == buf.len);

    if (c_str == NULL) {
        /* When input is NULL, length and capacity are zero and buffer is NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        /* When input is non‑NULL, length equals the string length (len) */
        assert(buf.len == len);
        /* buffer pointer is NULL iff capacity is zero (i.e., empty string) */
        if (buf.capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer == (uint8_t *)c_str);
        }
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
