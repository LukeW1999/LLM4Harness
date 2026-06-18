#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        str->allocator = NULL;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    size_t old_len = (str) ? str->len : 0;

    aws_string_destroy_secure(str);

    if (str) {
        const uint8_t *bytes = aws_string_bytes(str);
        for (size_t i = 0; i < old_len; ++i) {
            assert(bytes[i] == 0);
        }
        assert(str->len == old_len);
        assert(str->allocator == NULL);
        assert(aws_string_is_valid(str));
    } else {
        assert(str == NULL);
    }
}
