#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    /* allocator – use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic C string length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate a buffer for the C string (including null terminator) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the string with nondeterministic bytes (no early null) */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0'; /* null‑terminate */

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑conditions */
    if (result != NULL) {
        /* the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* length must equal the original C‑string length */
        assert(result->len == len);

        /* the bytes must match the original C‑string contents */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* allocator field must be the one passed in */
        assert(result->allocator == allocator);

        /* a null terminator is present immediately after the data */
        assert(result->bytes[len] == 0);
    } else {
        /* on failure the original C‑string must remain unchanged */
        assert(c_str[len] == '\0');
    }

    /* clean up */
    free(c_str);
    if (result) {
        aws_string_destroy(result);
    }
}
