#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* 1. Allocate and bound inputs */
    struct aws_allocator *allocator = aws_default_allocator();

    size_t max_len = MAX_BUFFER_SIZE;
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < max_len);

    char *c_str = (char *)malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* nondet fill the string (including possible null bytes before terminator) */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[str_len] = '\0';

    /* assume the C string is valid according to library predicate */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* 2. Save old state of inputs (no mutable fields, but keep for completeness) */
    struct aws_allocator *old_allocator = allocator;
    char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Postconditions */
    if (result != NULL) {
        /* result must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* allocator field must be the same as the one passed in */
        assert(result->allocator == allocator);

        /* length must match the original C string length */
        assert(result->len == str_len);

        /* bytes must match the original C string content */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, str_len);
    } else {
        /* on failure, no allocation was made; result is NULL */
        assert(result == NULL);
    }

    /* 5. Unchanged inputs */
    assert(allocator == old_allocator);
    assert(c_str == old_c_str);
    /* the content of c_str must remain unchanged */
    assert_bytes_match((const uint8_t *)c_str, (const uint8_t *)old_c_str, str_len + 1);

    /* 6. Validity of inputs after call */
    assert(aws_c_string_is_valid(c_str));

    /* clean up */
    free(c_str);
}
