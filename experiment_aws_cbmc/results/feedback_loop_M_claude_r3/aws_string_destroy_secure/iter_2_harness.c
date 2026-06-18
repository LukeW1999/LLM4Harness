#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Helper to create a valid aws_string with allocated memory */
struct aws_string *make_aws_string_for_test(struct aws_allocator *allocator, size_t len) {
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    for (size_t i = 0; i < len; i++) {
        *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
    }
    *(uint8_t *)&str->bytes[len] = 0;
    return str;
}

void aws_string_destroy_secure_harness(void) {
    bool is_null = nondet_bool();

    if (is_null) {
        aws_string_destroy_secure(NULL);
        assert(true);
    } else {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        bool has_allocator = nondet_bool();
        struct aws_allocator *allocator = has_allocator ? aws_default_allocator() : NULL;

        struct aws_string *str = make_aws_string_for_test(allocator, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        bool will_free = (old_allocator != NULL);

        if (!will_free) {
            aws_string_destroy_secure(str);

            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(str->allocator == NULL);

            free(str);
        } else {
            aws_string_destroy_secure(str);
            assert(true);
        }
    }
}
