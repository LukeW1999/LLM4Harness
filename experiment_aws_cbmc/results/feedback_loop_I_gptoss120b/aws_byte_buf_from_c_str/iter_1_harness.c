#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Nondeterministically choose a C string (may be NULL) */
    const char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* Choose a bounded length for the string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string plus terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Fill the allocated buffer with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
        }
        /* NUL‑terminate */
        ((uint8_t *)c_str)[len] = '\0';
    }

    /* 2. Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        /* Compute the actual length of the string */
        size_t computed_len = 0;
        while (c_str[computed_len] != '\0') {
            computed_len++;
        }
        assert(buf.len == computed_len);
        assert(buf.capacity == computed_len);
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* Allocator is always NULL for this constructor */
    assert(buf.allocator == NULL);

    /* 4. Structural validity must hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* 5. Clean up */
    if (c_str != NULL) {
        free((void *)c_str);
    }
}
