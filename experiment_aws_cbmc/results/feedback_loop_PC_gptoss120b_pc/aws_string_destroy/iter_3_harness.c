#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)storage;

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_bytes[MAX_STRING_LEN];

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        if (old_len > 0) {
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
        }
    }

    int performed_free = (str != NULL && str->allocator != NULL);

    aws_string_destroy(str);

    if (!performed_free) {
        if (str != NULL) {
            __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
            __CPROVER_assert(str->len == old_len, "length unchanged");
            if (old_len > 0) {
                for (size_t i = 0; i < old_len; ++i) {
                    __CPROVER_assert(((uint8_t *)str->bytes)[i] == old_bytes[i],
                                     "bytes unchanged");
                }
            }
            __CPROVER_assert(aws_string_is_valid(str), "string remains valid");
        }
    }
}
