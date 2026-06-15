#include <aws/common/string.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *s = NULL;

    if (allocator != NULL) {
        uint8_t data[MAX_STRING_LEN];
        for (size_t i = 0; i < len; ++i) {
            data[i] = (uint8_t)nondet_uint8_t();
        }
        s = aws_string_new_from_array(allocator, data, len);
    } else {
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } wrapper;

        wrapper.s.allocator = NULL;
        wrapper.s.len = len;
        for (size_t i = 0; i < len; ++i) {
            wrapper.bytes[i] = (uint8_t)nondet_uint8_t();
        }
        s = &wrapper.s;
    }

    aws_string_destroy_secure(s);

    if (allocator != NULL) {
        /* allocator may have freed memory; no further checks */
    } else {
        assert(s->allocator == NULL);
        assert(s->len == len);
    }
}
