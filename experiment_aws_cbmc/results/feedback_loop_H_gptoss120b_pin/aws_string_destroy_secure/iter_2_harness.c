#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    /* nondet decide whether to have a string */
    if (nondet_bool()) {
        /* nondet length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* stack‑allocated struct with flexible array space */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } storage;

        str = &storage.s;
        str->len = len;
        str->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* nondet fill the bytes */
        uint8_t *b = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            b[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (str) {
        struct aws_allocator *old_alloc = str->allocator;
        size_t old_len = str->len;

        aws_string_destroy_secure(str);

        if (old_alloc == NULL) {
            uint8_t *b = (uint8_t *)aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(b[i] == 0);
            }
            assert(str->allocator == NULL);
            assert(str->len == old_len);
            assert(aws_string_is_valid(str));
        }
    } else {
        aws_string_destroy_secure(str);
    }
}
