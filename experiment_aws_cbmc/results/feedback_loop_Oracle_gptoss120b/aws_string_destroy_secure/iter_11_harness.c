#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* Align buffer to satisfy struct aws_string alignment requirements */
    uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN] __attribute__((aligned(__alignof__(struct aws_string))));

    struct aws_string *str = nondet_bool()
        ? (struct aws_string *)NULL
        : (struct aws_string *)buffer;

    if (str) {
        /* Use NULL allocator to avoid freeing stack memory */
        str->allocator = NULL;
        str->len = len;
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t j = 0; j < len; ++j) {
            bytes[j] = nondet_uint8_t();
        }
    }

    size_t old_len = 0;
    if (str) {
        old_len = str->len;
        if (old_len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old_len);
            (void)i; /* suppress unused variable warning */
        }
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    aws_string_destroy_secure(str);

    if (str) {
        if (str->allocator == NULL) {
            if (old_len > 0) {
                size_t i = nondet_size_t();
                __CPROVER_assume(i < old_len);
                assert(((uint8_t *)str->bytes)[i] == 0);
            }
            assert(str->len == old_len);
        } else {
            assert(str->len == old_len);
        }
    } else {
        assert(str == NULL);
    }
}
