#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    const char *c_str;
    size_t len = nondet_size_t();
    c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[len] = '\0';

    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->len == len);
        assert(memcmp(str->bytes, c_str, len) == 0);
        aws_string_destroy(str);
    } else {
        assert(str == NULL);
    }

    free(c_str);

    // Test with NULL c_str
    str = aws_string_new_from_c_str(allocator, NULL);
    assert(str == NULL);

    // Test with empty string
    c_str = "";
    str = aws_string_new_from_c_str(allocator, c_str);
    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->len == 0);
        aws_string_destroy(str);
    } else {
        assert(str == NULL);
    }
}
