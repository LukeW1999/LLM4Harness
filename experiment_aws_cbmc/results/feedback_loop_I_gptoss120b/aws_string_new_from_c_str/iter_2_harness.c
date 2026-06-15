#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    /* allocator must be a valid allocator (use default) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministically choose a length for the input C string, bounded */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len < max_len);

    /* allocate a buffer for the C string and make it null‑terminated */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');   // ensure no interior null bytes
    }
    c_str[len] = '\0';

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑conditions */
    if (result != NULL) {
        /* length must equal the length of the input C string */
        assert(result->len == len);

        /* allocator field must be the allocator passed in */
        assert(result->allocator == allocator);

        /* the bytes of the new string must match the input bytes */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* a null terminator must exist immediately after the data */
        assert(result->bytes[len] == 0);

        /* the resulting string must satisfy its validity predicate */
        assert(aws_string_is_valid(result));
    } else {
        /* on allocation failure the function returns NULL; no further state changes */
        assert(result == NULL);
    }

    /* clean up */
    free(c_str);
}
