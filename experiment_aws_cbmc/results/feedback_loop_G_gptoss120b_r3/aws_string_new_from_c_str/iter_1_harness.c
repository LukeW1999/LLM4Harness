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
    /* 1. nondeterministic length for the input C string, bounded */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len < max_len);

    /* 2. allocate a C string with a terminating NUL */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0';

    /* 3. remember the original contents of c_str */
    struct store_byte_from_buffer c_str_old;
    save_byte_from_array((const uint8_t *)c_str, len, &c_str_old);

    /* 4. allocator (use default allocator as required) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 6. post‑condition checks */
    if (result != NULL) {
        /* result must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* length must match the length of the input C string */
        assert(result->len == len);

        /* bytes must be identical to the input C string (first len bytes) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* allocator field must be the allocator passed in */
        assert(result->allocator == allocator);

        /* a NUL terminator is guaranteed to exist immediately after the data */
        assert(result->bytes[len] == '\0');
    }

    /* 7. c_str must remain unchanged regardless of success or failure */
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_old);

    /* 8. clean up */
    free(c_str);
}
