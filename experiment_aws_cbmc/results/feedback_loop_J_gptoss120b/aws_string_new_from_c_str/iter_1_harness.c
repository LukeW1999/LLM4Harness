#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_C_STR_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* 1. Allocate and nondeterministically initialize a C string */
    char *c_str = malloc(MAX_C_STR_LEN);
    __CPROVER_assume(c_str != NULL);

    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_C_STR_LEN);               /* bound length */
    /* fill the string bytes with nondet values */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[str_len] = '\0';                                   /* null‑terminate */

    /* Save old state of the input string for immutability checks */
    struct store_byte_from_buffer c_str_old;
    save_byte_from_array((uint8_t *)c_str, str_len + 1, &c_str_old);

    /* 2. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(aws_default_allocator(), c_str);

    /* 3. Post‑condition checks */
    if (result != NULL) {
        /* the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* length must equal the original C‑string length */
        assert(result->len == str_len);

        /* the bytes must match the original C‑string contents (excluding the terminator) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, str_len);

        /* allocator field must be the allocator we passed */
        assert(result->allocator == aws_default_allocator());
    } else {
        /* on failure the result is NULL – no further checks on result */
        assert(result == NULL);
    }

    /* 4. The input C‑string must remain unchanged regardless of success or failure */
    assert_byte_from_buffer_matches((uint8_t *)c_str, &c_str_old);

    /* 5. Clean up */
    free(c_str);
}
