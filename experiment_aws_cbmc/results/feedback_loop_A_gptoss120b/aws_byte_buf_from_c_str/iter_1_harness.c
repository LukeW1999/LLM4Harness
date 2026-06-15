#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministic input string (may be NULL) */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* bound the length of the string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate space for the string plus terminating NUL */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);               /* allocation must succeed for the harness */
        /* fill the string with nondet data */
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = nondet_uint8_t();
        }
        tmp[len] = '\0';                              /* ensure NUL termination */
        c_str = tmp;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */

    /* allocator is always NULL */
    assert(buf.allocator == NULL);

    if (c_str == NULL) {
        /* When input is NULL, length and capacity are zero and buffer is NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        /* When input is non‑NULL, length equals the string length */
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
