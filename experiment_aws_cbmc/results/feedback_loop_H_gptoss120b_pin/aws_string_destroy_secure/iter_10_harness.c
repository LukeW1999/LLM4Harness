#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        union {
            struct aws_string s;
            uint8_t raw[sizeof(struct aws_string) + MAX_STRING_LEN];
        } storage;

        str = &storage.s;
        str->len = len;
        str->allocator = NULL; /* ensure no free is attempted */

        uint8_t *b = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            b[i] = nondet_uint8_t();
        }
    }

    if (str) {
        size_t old_len = str->len;

        aws_string_destroy_secure(str);

        uint8_t *b = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < old_len; ++i) {
            assert(b[i] == 0);
        }
        assert(str->allocator == NULL);
        assert(str->len == old_len);
    } else {
        aws_string_destroy_secure(str);
    }
}
