#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LENGTH 32

void aws_string_destroy_secure_harness(void) {
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;
    const uint8_t *old_bytes = NULL;
    size_t old_len = 0;
    bool has_allocator = false;

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LENGTH);

        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = (struct aws_string *)can_fail_malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        struct aws_allocator *allocator = nondet_bool() ? aws_default_allocator() : NULL;
        str->allocator = allocator;

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));

        if (allocator == NULL) {
            old_bytes = aws_string_bytes(str);
            old_len = len;
        } else {
            has_allocator = true;
        }
    }

    aws_string_destroy_secure(str);

    if (!is_null && !has_allocator) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == 0);
        }
    }
}
