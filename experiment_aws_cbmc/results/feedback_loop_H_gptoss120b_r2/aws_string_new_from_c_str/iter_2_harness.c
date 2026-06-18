#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_new_from_c_str_harness(void) {
    /* allocator: use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministically choose a length for the input C string, bounded */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= max_len);

    /* allocate a buffer for the C string (including space for the null terminator) */
    char *c_str = (char *)malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the buffer with nondeterministic non‑zero bytes and terminate it */
    for (size_t i = 0; i < c_len; ++i) {
        uint8_t val = nondet_uint8_t();
        __CPROVER_assume(val != 0);
        c_str[i] = (char)val;
    }
    c_str[c_len] = '\0';

    /* save a copy of the input bytes (including the terminator) for later comparison */
    struct store_byte_from_buffer c_str_old;
    save_byte_from_array((const uint8_t *)c_str, c_len + 1, &c_str_old);

    /* call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑condition checks */
    if (result != NULL) {
        /* the newly created string must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* allocator field must be the allocator that was passed in */
        assert(result->allocator == allocator);

        /* length must equal the length of the input C string */
        assert(result->len == c_len);

        /* the data bytes must match the input bytes (excluding the terminating '\0') */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_len);
    }

    /* input C string must be unchanged regardless of success or failure */
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &c_str_old);

    /* clean up */
    if (result != NULL) {
        aws_string_destroy(result);
    }
    free(c_str);
}
