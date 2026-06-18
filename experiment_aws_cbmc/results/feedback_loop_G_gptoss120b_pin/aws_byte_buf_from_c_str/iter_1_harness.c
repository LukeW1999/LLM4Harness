#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministic choice whether the input pointer is NULL */
    bool is_null = nondet_bool();

    const char *c_str = NULL;
    size_t expected_len = 0;

    if (!is_null) {
        /* nondeterministic length of the string (excluding the terminating NUL) */
        expected_len = nondet_size_t();
        __CPROVER_assume(expected_len <= MAX_BUFFER_SIZE);

        /* allocate a buffer large enough for the string plus terminating NUL */
        char *tmp = malloc(expected_len + 1);
        __CPROVER_assume(tmp != NULL);

        /* fill the string with nondeterministic bytes */
        for (size_t i = 0; i < expected_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        /* NUL‑terminate */
        tmp[expected_len] = '\0';

        c_str = tmp;
    } else {
        c_str = NULL;
        expected_len = 0;
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* Post‑conditions */
    assert(result.len == expected_len);
    assert(result.capacity == expected_len);
    assert(result.allocator == NULL);

    if (expected_len == 0) {
        assert(result.buffer == NULL);
    } else {
        assert(result.buffer == (uint8_t *)c_str);
    }

    /* Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&result));
}
