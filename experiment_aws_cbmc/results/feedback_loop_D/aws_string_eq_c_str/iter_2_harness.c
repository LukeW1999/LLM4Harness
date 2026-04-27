#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_c_str_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string));
    __CPROVER_assume(aws_string_is_valid(str));
    const char *c_str = (const char *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);

    // Assume bytes points to a valid memory region and is null-terminated
    size_t len = str->len;
    char *bytes = (char *)malloc(len + 1);
    __CPROVER_assume(bytes != NULL);
    bytes[len] = '\0';
    str->bytes = bytes;

    struct aws_string old_str = *str;
    const char *old_c_str = c_str;

    bool result = aws_string_eq_c_str(str, c_str);

    // Step 1: Success path assertions
    if (result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
    }

    // Step 2: Failure path assertions
    if (!result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
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
