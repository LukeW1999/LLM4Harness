#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t len = nondet_size_t();
    __CPROVER_assume(len < 256);

    uint8_t data[256];
    for (size_t i = 0; i < len; ++i) {
        data[i] = nondet_uint8_t();
    }
    data[len] = '\0';

    struct aws_string *str = aws_string_new_from_array(allocator, data, len);

    if (str) {
        size_t old_len = str->len;
        aws_string_destroy_secure(str);
        (void)old_len; /* suppress unused variable warning */
    }
}
