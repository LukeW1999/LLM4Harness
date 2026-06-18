#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LENGTH 32

/* Mutable version of aws_string to allow field assignment on freshly allocated memory */
struct aws_string_mutable {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[];
};

void aws_string_destroy_secure_harness(void) {
    bool is_null;
    struct aws_string *str = NULL;
    const uint8_t *old_bytes = NULL;
    size_t old_len = 0;
    bool has_allocator = false;

    if (!is_null) {
        bool use_allocator;
        if (use_allocator) {
            size_t len;
            __CPROVER_assume(len <= MAX_STRING_LENGTH);
            struct aws_allocator *allocator = aws_default_allocator();
            str = ensure_aws_string_is_allocated(allocator, len);
            __CPROVER_assume(str != NULL);
            has_allocator = true;
        } else {
            size_t len;
            __CPROVER_assume(len <= MAX_STRING_LENGTH);
            size_t alloc_size = sizeof(struct aws_string) + len + 1;
            str = (struct aws_string *)can_fail_malloc(alloc_size);
            __CPROVER_assume(str != NULL);

            struct aws_string_mutable *mut = (struct aws_string_mutable *)str;
            mut->allocator = NULL;
            mut->len = len;
            for (size_t i = 0; i < len; ++i) {
                uint8_t byte;
                mut->bytes[i] = byte;
            }
            mut->bytes[len] = 0;

            old_bytes = aws_string_bytes(str);
            old_len = len;
            has_allocator = false;
        }
    }

    aws_string_destroy_secure(str);

    if (!is_null && !has_allocator) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == 0);
        }
    }
}
