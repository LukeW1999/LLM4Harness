#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* allocator – use the default allocator (never NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic length of the input C string, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate a nondeterministic C string (including terminating NUL) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0';

    /* Save a copy of the original bytes for immutability checks */
    uint8_t *c_str_copy = malloc(len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    for (size_t i = 0; i <= len; ++i) {
        c_str_copy[i] = (uint8_t)c_str[i];
    }

    /* Call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Post‑condition: result is either a valid string or NULL */
    if (result != NULL) {
        /* The returned string must be valid */
        assert(aws_string_is_valid(result));

        /* Length must match the input length */
        assert(result->len == len);

        /* Allocator must be the one we passed */
        assert(result->allocator == allocator);

        /* The bytes must be an exact copy of the input (excluding the NUL) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* A NUL terminator must exist immediately after the data */
        assert(result->bytes[len] == 0);
    } else {
        /* On failure the function returns NULL */
        assert(result == NULL);
    }

    /* The input C string must remain unchanged */
    for (size_t i = 0; i <= len; ++i) {
        assert((uint8_t)c_str[i] == c_str_copy[i]);
    }

    /* The allocator pointer is unchanged (trivial, but required) */
    assert(allocator == aws_default_allocator());

    /* Clean up the nondeterministic buffers */
    free(c_str);
    free(c_str_copy);
}
