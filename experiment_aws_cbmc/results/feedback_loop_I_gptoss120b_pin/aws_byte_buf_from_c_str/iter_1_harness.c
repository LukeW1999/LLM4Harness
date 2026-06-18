#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* allocate a non‑empty buffer with a bounded size */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);

        /* fill with nondet data and terminate */
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        buf[len] = '\0';               /* ensure null‑termination */
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */
    size_t expected_len = (c_str == NULL) ? 0 : strlen(c_str);

    /* length and capacity must equal the string length */
    assert(result.len == expected_len);
    assert(result.capacity == expected_len);

    /* buffer pointer handling */
    if (expected_len == 0) {
        assert(result.buffer == NULL);
    } else {
        assert(result.buffer == (uint8_t *)c_str);
    }

    /* allocator is always NULL */
    assert(result.allocator == NULL);

    /* 4. Validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
