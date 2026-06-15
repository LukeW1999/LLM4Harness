#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE); /* leave room for null terminator */

        uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;

        /* Use a static string (no allocator) to avoid freeing stack memory */
        str->allocator = NULL;

        str->len = len;

        const uint8_t *bytes_const = aws_string_bytes(str);
        uint8_t *bytes = (uint8_t *)bytes_const;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* null‑terminate */

        __CPROVER_assume(aws_string_is_valid(str));

        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;

        aws_string_destroy_secure(str);

        for (size_t i = 0; i < len; ++i) {
            __CPROVER_assert(bytes[i] == 0,
                             "aws_string_destroy_secure: bytes are zeroed");
        }

        __CPROVER_assert(str->len == old_len,
                         "aws_string_destroy_secure: len unchanged");
        __CPROVER_assert(str->allocator == old_allocator,
                         "aws_string_destroy_secure: allocator unchanged");

        if (str->allocator == NULL) {
            __CPROVER_assert(aws_string_is_valid(str),
                             "aws_string_destroy_secure: static string remains valid");
        }
    } else {
        str = NULL;
        aws_string_destroy_secure(str);
    }
}
