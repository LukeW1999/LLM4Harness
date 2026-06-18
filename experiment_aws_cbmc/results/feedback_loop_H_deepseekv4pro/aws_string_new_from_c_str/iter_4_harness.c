#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

#define MAX_STRING_LEN 16

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    const char *c_str;
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(strlen(c_str) <= MAX_STRING_LEN);

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        __CPROVER_assume(__CPROVER_r_ok(result->bytes, result->len + 1));
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == strlen(c_str));
        /* Ensure c_str is readable for the length we are comparing */
        __CPROVER_assume(__CPROVER_r_ok(c_str, result->len + 1));
        assert(memcmp(result->bytes, c_str, result->len) == 0);
        assert(result->bytes[result->len] == '\0');
    } else {
        assert(result == NULL);
    }
}
