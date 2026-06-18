#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    char *c_str = NULL;
    __CPROVER_assume(c_str == NULL || nondet_bool());

    if (c_str != NULL) {
        /* bound the length of the string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate space for the string plus terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill with nondet bytes and terminate */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[len] = '\0';
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Expected length */
    size_t expected_len = (c_str == NULL) ? 0 : strlen(c_str);

    /* 4. Post‑condition asserts */
    assert(buf.len == expected_len);
    assert(buf.capacity == expected_len);
    if (expected_len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
    }
    assert(buf.allocator == NULL);

    /* 5. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* 6. Clean up */
    free(c_str);
}
