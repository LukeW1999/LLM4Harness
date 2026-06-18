#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (__CPROVER_nondet_bool()) {
        uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)buffer;

        if (__CPROVER_nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = (size_t)__CPROVER_nondet_uint64_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_first_byte = 0;

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        if (old_len > 0) {
            old_first_byte = str->bytes[0];
        }
    }

    aws_string_destroy(str);

    if (str != NULL) {
        if (old_allocator != NULL) {
            /* memory may have been released; no further checks */
        } else {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            if (old_len > 0) {
                assert(str->bytes[0] == old_first_byte);
            }
        }
    }

    if (str != NULL && old_allocator == NULL) {
        /* When allocator is NULL, aws_string_destroy should be a no‑op,
           so the string remains valid. */
        assert(str->len == old_len);
        if (old_len > 0) {
            assert(str->bytes[0] == old_first_byte);
        }
    }
}
