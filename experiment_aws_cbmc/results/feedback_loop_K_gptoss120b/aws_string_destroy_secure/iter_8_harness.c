#include <aws/common/string.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct aws_allocator *allocator = aws_default_allocator();

    struct {
        struct aws_string s;
        char bytes[MAX_STRING_LEN];
    } wrapper;

    wrapper.s.allocator = allocator;
    wrapper.s.len = len;

    /* initialize the string contents with nondet data and null‑terminate */
    for (size_t i = 0; i < len; ++i) {
        wrapper.bytes[i] = (char)nondet_uint8_t();
    }
    wrapper.bytes[len] = '\0';

    aws_string_destroy_secure(&wrapper.s);

    if (allocator != NULL) {
        /* allocator may have freed memory; no further checks */
    } else {
        assert(wrapper.s.allocator == NULL);
        assert(wrapper.s.len == len);
    }
}
