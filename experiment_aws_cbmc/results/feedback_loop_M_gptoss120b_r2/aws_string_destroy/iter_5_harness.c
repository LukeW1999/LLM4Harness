#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;
    bool have_str = nondet_bool();

    if (have_str) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_allocator *allocator = aws_default_allocator();
        str = allocator->alloc(allocator, sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy(str);
}
