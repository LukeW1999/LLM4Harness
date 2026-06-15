#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    const char *c_str = (const char *)nondet_uint8_t();
    size_t c_str_len = nondet_size_t();

    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    struct aws_string *old_str = NULL;
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->allocator == allocator);
        assert(str->len == c_str_len);
        assert_bytes_match((const uint8_t *)c_str, str->bytes, c_str_len);
    } else {
        assert(old_str == NULL);
    }
}
