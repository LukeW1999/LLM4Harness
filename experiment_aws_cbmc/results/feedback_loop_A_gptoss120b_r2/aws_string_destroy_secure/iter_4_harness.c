#include <aws/common/byte_buf.h>
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

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_string *old_str = str;
    size_t old_len = (str) ? str->len : 0;
    struct aws_allocator *old_allocator = (str) ? str->allocator : NULL;

    aws_string_destroy_secure(str);

    if (str) {
        if (old_allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
            assert(str->len == old_len);
            assert(str->allocator == NULL);
            assert(str == old_str);
            assert(aws_string_is_valid(str));
        } else {
            (void)old_len; /* silence unused‑variable warning */
        }
    } else {
        assert(str == NULL);
    }
}
