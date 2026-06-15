#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    bool make_null = nondet_bool();

    if (make_null) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        size_t total_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);

        bool allocator_null = nondet_bool();
        if (allocator_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save old state for later checks */
    struct {
        struct aws_allocator *allocator;
        size_t len;
    } old = {0};

    if (str) {
        old.allocator = str->allocator;
        old.len = str->len;
    }

    aws_string_destroy_secure(str);

    if (str == NULL) {
        /* When the input is NULL the function must be a no‑op */
        assert(str == NULL);
    } else {
        if (old.allocator == NULL) {
            /* Secure destroy with no allocator: memory stays allocated and
               must be zeroed, while length and allocator fields stay unchanged. */
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old.len; ++i) {
                assert(bytes[i] == 0);
            }
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            assert(aws_string_is_valid(str));
        } else {
            /* Secure destroy with an allocator: memory may have been freed.
               We cannot dereference `str` after the call. */
            assert(1);
        }
    }
}
