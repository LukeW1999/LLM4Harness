#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define MAX_STRING_LEN 16

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    const char *c_str = ensure_c_str_is_allocated(MAX_STRING_LEN);
    size_t c_len = strlen(c_str);

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_len);
        assert(memcmp(result->bytes, c_str, c_len) == 0);
        assert(result->bytes[c_len] == '\0');
    } else {
        assert(result == NULL);
    }
}
