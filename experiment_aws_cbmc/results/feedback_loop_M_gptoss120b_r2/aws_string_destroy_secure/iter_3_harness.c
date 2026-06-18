#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    const size_t MAX_LEN = 256;
    uint8_t storage[sizeof(struct aws_string) + MAX_LEN];

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);

        str = (struct aws_string *)storage;

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    aws_string_destroy_secure(str);

    if (str && old_allocator == NULL) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == 0);
        }
        assert(aws_string_is_valid(str));
    }
}
