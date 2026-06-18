#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet length bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < 1024);

    /* nondet buffer on the stack */
    uint8_t buf[1024];
    for (size_t j = 0; j < len; ++j) {
        buf[j] = nondet_uint8_t();
    }

    /* allocate a string using the allocator */
    struct aws_string *str = aws_string_new_from_array(alloc, buf, len);
    if (str) {
        /* nondet choice of allocator stored in the string */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = alloc;
        }
    }

    struct aws_string *old_str = str;
    size_t i = nondet_size_t();

    __CPROVER_assume(old_str == NULL || aws_string_is_valid(old_str));
    __CPROVER_assume(old_str == NULL || i < old_str->len);

    aws_string_destroy_secure(str);

    if (old_str != NULL) {
        uint8_t *bytes = (uint8_t *)aws_string_bytes(old_str);
        assert(bytes[i] == 0);
    }
}
