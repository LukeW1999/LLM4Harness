#include <aws/common/string.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    bool has_allocator = nondet_bool();
    struct aws_allocator *allocator = has_allocator ? aws_default_allocator() : NULL;

    struct {
        struct aws_string s;
        char bytes[MAX_STRING_LEN];
    } wrapper;

    wrapper.s.allocator = allocator;
    wrapper.s.len = len;

    __CPROVER_assume(aws_string_is_valid(&wrapper.s));

    aws_string_destroy_secure(&wrapper.s);

    if (allocator != NULL) {
        /* memory released, no further access */
    } else {
        assert(wrapper.s.allocator == NULL);
        assert(wrapper.s.len == len);
    }
}
