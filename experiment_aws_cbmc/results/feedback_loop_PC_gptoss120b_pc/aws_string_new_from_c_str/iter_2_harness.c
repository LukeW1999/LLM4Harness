#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* allocator (use default allocator) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic string length, bounded */
    const size_t MAX_STR_LEN = 256;
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_STR_LEN);

    /* allocate a C string with space for null terminator */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the string with nondeterministic non‑zero bytes and null‑terminate */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[str_len] = '\0';

    /* snapshot inputs for immutability checks */
    struct aws_allocator *old_allocator = allocator;
    char *old_c_str = c_str;

    /* keep a copy of the original bytes for later comparison */
    uint8_t *c_str_copy = malloc(str_len);
    __CPROVER_assume(c_str_copy != NULL);
    for (size_t i = 0; i < str_len; ++i) {
        c_str_copy[i] = (uint8_t)c_str[i];
    }

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑condition checks */
    if (result != NULL) {
        /* the newly created string must be valid */
        assert(aws_string_is_valid(result));

        /* allocator stored in the string must be the one we passed */
        assert(result->allocator == allocator);

        /* length must equal the length of the input C string */
        assert(result->len == str_len);

        /* the bytes of the string must match the original C string bytes */
        assert_bytes_match(result->bytes, c_str_copy, str_len);

        /* a null terminator must be present immediately after the data */
        assert(result->bytes[str_len] == 0);
    } else {
        /* on failure the function returns NULL */
        assert(result == NULL);
    }

    /* inputs must remain unchanged */
    assert(allocator == old_allocator);
    assert(c_str == old_c_str);
    for (size_t i = 0; i < str_len; ++i) {
        assert(c_str[i] == (char)c_str_copy[i]);
    }
    assert(c_str[str_len] == '\0');

    /* clean up */
    free(c_str_copy);
    free(c_str);
}
