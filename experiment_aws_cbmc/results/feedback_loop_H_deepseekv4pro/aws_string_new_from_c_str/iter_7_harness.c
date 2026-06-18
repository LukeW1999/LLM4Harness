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

    const char *c_str;
    __CPROVER_assume(c_str != NULL);
    /* Ensure the entire region up to MAX_STRING_LEN+1 is readable */
    __CPROVER_assume(__CPROVER_r_ok(c_str, MAX_STRING_LEN + 1));

    size_t c_len = strlen(c_str);
    __CPROVER_assume(c_len <= MAX_STRING_LEN);

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_len);

        /* Re‑establish readability for both buffers before memcmp */
        __CPROVER_assume(__CPROVER_r_ok(c_str, c_len));
        __CPROVER_assume(__CPROVER_r_ok(result->bytes, result->len));

        assert(memcmp(result->bytes, c_str, c_len) == 0);
        assert(result->bytes[c_len] == '\0');
    } else {
        assert(result == NULL);
    }
}
