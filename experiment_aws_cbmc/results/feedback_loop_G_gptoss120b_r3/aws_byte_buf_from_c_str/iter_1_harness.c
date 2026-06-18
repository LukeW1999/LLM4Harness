#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Non‑deterministically decide whether the input string is NULL */
    const char *c_str;
    bool is_null = nondet_bool();

    size_t str_len = 0;               /* length of the string when not NULL */
    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded buffer for the string */
        size_t max_len = MAX_BUFFER_SIZE;
        char *buf = malloc(max_len);
        __CPROVER_assume(buf != NULL);

        /* Fill the buffer with nondeterministic bytes */
        for (size_t i = 0; i < max_len; ++i) {
            ((uint8_t *)buf)[i] = nondet_uint8_t();
        }

        /* Choose a nondeterministic position for the terminating NUL byte */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len < max_len);
        buf[str_len] = '\0';

        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */

    /* allocator must always be NULL */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* When input is NULL, all fields must be zero/NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* When input is a valid C string */
        assert(result.len == str_len);
        assert(result.capacity == str_len);
        assert(result.buffer == (uint8_t *)c_str);
    }

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&result));
}
