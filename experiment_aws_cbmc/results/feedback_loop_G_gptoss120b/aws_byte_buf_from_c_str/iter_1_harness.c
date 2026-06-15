#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Nondeterministic input string (may be NULL) */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a nondeterministic sized, null‑terminated string */
        size_t max_len = MAX_BUFFER_SIZE;               /* bound for the string length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < max_len);

        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[len] = '\0';                               /* ensure null‑termination */
        c_str = tmp;
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */

    if (c_str == NULL) {
        /* When input is NULL, all fields must be zero / NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);

        /* Length and capacity must equal the string length */
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);

        /* Buffer must point to the original string (cast to uint8_t *) */
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* Allocator is always NULL for this constructor */
    assert(buf.allocator == NULL);

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
