#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#define MAX_STRING_LEN 1024

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    const char *c_str = nd_real_string();
    __CPROVER_assume(c_str != NULL);
    size_t len = strlen(c_str);
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == len);
        assert(memcmp(result->bytes, c_str, len) == 0);
        assert(aws_string_is_valid(result));
    }

    aws_string_destroy(result);
}
