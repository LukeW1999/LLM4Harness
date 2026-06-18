#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_LEN 256

void aws_string_destroy_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_LEN);

    uint8_t storage[sizeof(struct aws_string) + MAX_LEN];
    struct aws_string *str = (struct aws_string *)storage;

    str->allocator = aws_default_allocator();
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    aws_string_destroy(str);
}
