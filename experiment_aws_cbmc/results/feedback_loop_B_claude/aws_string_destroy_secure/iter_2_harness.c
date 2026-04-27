#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len;
        __CPROVER_assume(len <= 8);

        size_t alloc_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string));

        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* Initialize fields - cast away const */
        if (nondet_bool()) {
            *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        } else {
            *(struct aws_allocator **)&str->allocator = NULL;
        }
        *(size_t *)&str->len = len;

        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool str_is_null = (str == NULL);
    struct aws_allocator *old_allocator = str_is_null ? NULL : str->allocator;
    size_t old_len = str_is_null ? 0 : str->len;

    aws_string_destroy_secure(str);

    if (str_is_null) {
        assert(true);
    } else {
        if (old_allocator == NULL) {
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
        } else {
            assert(true);
        }
    }
}
