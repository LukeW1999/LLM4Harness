#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string));
    __CPROVER_assume(aws_string_is_valid(str));
    const char *c_str = (const char *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);

    // Assume bytes points to a valid memory region and is null-terminated
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE - 1); // Ensure room for null terminator
    char *bytes = (char *)malloc(len + 1);
    __CPROVER_assume(bytes != NULL);
    bytes[len] = '\0';
    str->bytes = bytes;
    str->len = len;

    // Fill c_str with some data
    for (size_t i = 0; i < len; i++) {
        c_str[i] = nondet_char();
    }
    c_str[len] = '\0';

    struct aws_string old_str = *str;
    const char *old_c_str = c_str;

    bool result = aws_string_eq_c_str(str, c_str);

    // Step 1: Success path assertions
    if (result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
        for (size_t i = 0; i < str->len; i++) {
            assert(str->bytes[i] == c_str[i]);
        }
    }

    // Step 2: Failure path assertions
    if (!result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
        // At least one character must differ
        bool any_diff = false;
        for (size_t i = 0; i < str->len; i++) {
            if (str->bytes[i] != c_str[i]) {
                any_diff = true;
                break;
            }
        }
        assert(any_diff);
    }

    // Step 3: Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes == old_str.bytes);
    assert(c_str == old_c_str);

    // Step 4: Validity invariants
    assert(aws_string_is_valid(str));

    free(bytes);
    free(str);
    free((void *)c_str);
}
