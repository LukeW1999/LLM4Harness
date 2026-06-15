#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    /* allocator: use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic length of the input C string, bounded */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* construct a valid C string of exactly c_str_len bytes (no early NUL) */
    char c_str_buf[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str_buf[i] != '\0');
    }
    c_str_buf[c_str_len] = '\0'; /* null terminator */

    const char *c_str = c_str_buf;

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* postconditions */
    if (result != NULL) {
        /* length must match the input string length */
        assert(result->len == c_str_len);

        /* bytes must be an exact copy of the input string */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);

        /* null terminator is present immediately after the data */
        assert(result->bytes[c_str_len] == '\0');

        /* allocator field must be the one we passed in */
        assert(result->allocator == allocator);

        /* the resulting string must satisfy its validity predicate */
        assert(aws_string_is_valid(result));
    } else {
        /* on allocation failure the function returns NULL */
        assert(result == NULL);
    }

    /* allocator itself is unchanged (trivial, as we did not modify it) */
    assert(allocator == aws_default_allocator());
}
