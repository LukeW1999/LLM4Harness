#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministically decide if the input string is NULL */
    bool is_null = nondet_bool();

    char *c_str = NULL;
    size_t len = 0;

    if (!is_null) {
        /* bound the length of the string */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate space for the string plus the terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill the string with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        /* null‑terminate */
        c_str[len] = '\0';
    } else {
        c_str = NULL;
        len = 0;
    }

    /* call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* post‑conditions */
    assert(result.allocator == NULL);
    assert(result.capacity == len);
    assert(result.len == len);
    if (len == 0) {
        assert(result.buffer == NULL);
    } else {
        assert(result.buffer == (uint8_t *)c_str);
    }

    /* validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
