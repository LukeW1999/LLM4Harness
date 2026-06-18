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
    /* 1. Allocate and nondeterministically initialize a C string */
    const size_t max_len = 20;                     /* bound for the string length */
    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* nondet fill the buffer */
    for (size_t i = 0; i < max_len + 1; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }

    /* ensure the buffer contains a null terminator within the bound */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* compute the actual length (up to the first null) */
    size_t c_str_len = 0;
    while (c_str_len < max_len && c_str[c_str_len] != '\0') {
        ++c_str_len;
    }

    /* save old state of the input string for later immutability checks */
    struct store_byte_from_buffer old_c_str_storage;
    save_byte_from_array((uint8_t *)c_str, c_str_len + 1, &old_c_str_storage);

    /* 2. Use the default allocator (never NULL for this harness) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Post‑condition checks */
    if (result != NULL) {
        /* success path */
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_str_len);
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);
        /* the null terminator is guaranteed to exist just after the data */
        assert(result->bytes[c_str_len] == '\0');
    } else {
        /* failure path – inputs must be unchanged */
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_storage);
    }

    /* 5. The allocator variable itself is unchanged (trivial for this harness) */
    assert(allocator == aws_default_allocator());

    /* 6. Clean up */
    free(c_str);
}
