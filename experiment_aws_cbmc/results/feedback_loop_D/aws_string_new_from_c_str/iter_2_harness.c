#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    const char *c_str = nd_real_string();
    __CPROVER_assume(c_str != NULL);

    struct aws_allocator old_allocator = *allocator;
    const char *old_c_str = c_str;

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, result->len) == 0);
        assert(aws_string_is_valid(result));
    } else {
        assert(*allocator == old_allocator);
        assert(c_str == old_c_str);
    }

    aws_string_destroy(result);
}
