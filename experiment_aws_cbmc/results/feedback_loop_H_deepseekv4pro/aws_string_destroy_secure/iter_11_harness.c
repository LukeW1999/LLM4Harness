#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    /* NULL case */
    aws_string_destroy_secure(NULL);

    /* Non-NULL case: stack-allocated string structure */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    uint8_t backing_buf[sizeof(struct aws_string) + MAX_LEN + 1];
    struct aws_string *str = (struct aws_string *)backing_buf;

    str->allocator = aws_default_allocator();
    str->len = len;
    str->bytes[len] = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t val;
        str->bytes[i] = val;
    }

    aws_string_destroy_secure(str);
}
