#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_new_from_c_str_harness() {
    /* allocator – use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic C string (bounded) */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len < max_len);

    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill with nondeterministic bytes */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0';

    /* assume the string is a valid C string */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑conditions */
    if (result != NULL) {
        /* the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* allocator field must be the one we passed */
        assert(result->allocator == allocator);

        /* length must equal the length of the input C string */
        assert(result->len == len);

        /* the bytes must match the input data */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* a null terminator follows the data bytes */
        assert(result->bytes[len] == '\0');
    } else {
        /* on failure, no observable state change (nothing to assert) */
    }

    free(c_str);
}
