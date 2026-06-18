#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* 1. Allocate and bound the allocator (use default allocator) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Create a nondeterministic C string */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= max_len);

    /* allocate buffer for the string including null terminator */
    char *c_str = (char *)malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill with nondeterministic bytes */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    /* null‑terminate */
    c_str[str_len] = '\0';

    /* 3. Save old state of the input string */
    uint8_t *c_str_old = (uint8_t *)malloc(str_len + 1);
    __CPROVER_assume(c_str_old != NULL);
    for (size_t i = 0; i <= str_len; ++i) {
        c_str_old[i] = (uint8_t)c_str[i];
    }

    /* 4. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 5. Post‑condition checks */
    if (result != NULL) {
        /* result must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* allocator must be preserved */
        assert(result->allocator == allocator);

        /* length must equal the original C‑string length */
        assert(result->len == str_len);

        /* bytes must match the original C‑string content (excluding the null terminator) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, str_len);
    } else {
        /* on failure, result is NULL */
        assert(result == NULL);
    }

    /* 6. Input C‑string must remain unchanged */
    for (size_t i = 0; i <= str_len; ++i) {
        assert((uint8_t)c_str[i] == c_str_old[i]);
    }

    /* 7. Clean up */
    free(c_str);
    free(c_str_old);
}
