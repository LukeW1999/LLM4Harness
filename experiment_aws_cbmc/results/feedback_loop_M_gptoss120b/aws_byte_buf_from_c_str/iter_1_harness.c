#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Nondeterministic input string (may be NULL) */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* 2. Bounded length for the string (excluding the terminating NUL) */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Allocate a buffer of len+1 bytes and make it a valid C string */
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[len] = '\0';
        c_str = buf;
    }

    /* 4. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 5. Post‑condition assertions */

    /* allocator is always set to NULL */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* When input is NULL, the buffer fields must be zero/NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* When input is non‑NULL, len and capacity equal the string length,
         * buffer points to the original string, and allocator remains NULL. */
        size_t expected_len = strlen(c_str);
        assert(expected_len <= MAX_BUFFER_SIZE);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }

    /* 6. Validity invariant must hold for the returned struct */
    assert(aws_byte_buf_is_valid(&result));
}
