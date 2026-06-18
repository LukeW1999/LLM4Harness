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
    /* 1. Allocate and bound inputs */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);

    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* nondet fill the buffer */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0'; /* ensure null‑termination */

    /* Save old state of inputs */
    struct aws_allocator *old_allocator = allocator;
    char *old_c_str = c_str;
    struct store_byte_from_buffer c_str_storage;
    save_byte_from_array((const uint8_t *)c_str, c_str_len, &c_str_storage);

    /* 2. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 3. Compute expected length (up to first null) */
    size_t expected_len = 0;
    while (c_str[expected_len] != '\0') {
        ++expected_len;
    }

    /* 4. Post‑condition checks */
    if (result != NULL) {
        /* result must be a valid string */
        assert(aws_string_is_valid(result));

        /* length must match the C‑string length */
        assert(result->len == expected_len);

        /* bytes must match the original C‑string bytes (excluding null) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, expected_len);

        /* allocator field must be the one passed in */
        assert(result->allocator == allocator);
    } else {
        /* on failure, inputs must be unchanged */
        assert(c_str == old_c_str);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_storage);
    }

    /* 5. Unchanged inputs (always true) */
    assert(allocator == old_allocator);
    assert(c_str == old_c_str);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_storage);
}
