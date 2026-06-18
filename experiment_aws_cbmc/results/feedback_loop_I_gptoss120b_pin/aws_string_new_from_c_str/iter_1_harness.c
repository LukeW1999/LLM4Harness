#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_new_from_c_str_harness() {
    /* 1. Allocate and bound inputs */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill with nondet non‑null bytes and terminate */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0';

    /* Assume the C string is valid according to library predicate */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* Save old state of inputs for immutability checks */
    struct store_byte_from_buffer old_c_str_storage;
    save_byte_from_array((const uint8_t *)c_str, len + 1, &old_c_str_storage);
    struct aws_allocator *old_allocator = allocator;

    /* 2. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 3. Post‑condition checks */
    if (result != NULL) {
        /* Success: result must be a valid string */
        assert(aws_string_is_valid(result));

        /* Length must equal the original C‑string length */
        assert(result->len == len);

        /* Allocator field must be the one passed in */
        assert(result->allocator == allocator);

        /* Bytes must match the original input up to len */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* Null terminator must be present immediately after the data */
        assert(result->bytes[len] == '\0');
    } else {
        /* Failure: result is NULL, no allocation performed */
        assert(result == NULL);
    }

    /* 4. Unchanged inputs */
    assert(allocator == old_allocator);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_storage);
}
