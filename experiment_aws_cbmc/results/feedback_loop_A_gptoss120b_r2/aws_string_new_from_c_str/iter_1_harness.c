#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* allocator – use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministically choose a length for the C string (bounded) */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* allocate space for the string plus the terminating NUL */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the string with nondeterministic bytes */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    /* ensure NUL termination */
    c_str[str_len] = '\0';

    /* Save old state of inputs that must remain unchanged */
    struct aws_allocator *old_allocator = allocator;
    char *old_c_str = c_str; /* pointer itself must not change */

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Post‑condition checks */
    if (result != NULL) {
        /* The returned string must be valid */
        assert(aws_string_is_valid(result));

        /* Its allocator must be the one we passed */
        assert(result->allocator == old_allocator);

        /* Length must equal the length of the input C string */
        assert(result->len == str_len);

        /* The bytes of the string must match the input bytes (excluding the NUL) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, str_len);
    } else {
        /* On failure the allocator and input pointer must be unchanged */
        assert(allocator == old_allocator);
        assert(c_str == old_c_str);
    }

    /* Clean up */
    free(c_str);
    if (result != NULL) {
        aws_string_destroy(result);
    }

    return 0;
}
