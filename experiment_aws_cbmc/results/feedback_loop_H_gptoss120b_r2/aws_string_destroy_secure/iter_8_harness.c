#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        alignas(struct aws_string) uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;

        /* Use a NULL allocator to avoid freeing stack memory */
        str->allocator = NULL;

        str->len = len;

        uint8_t *bytes = aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    if (str) {
        const struct aws_allocator *old_allocator = str->allocator;
        const size_t old_len = str->len;
        const bool old_allocator_is_null = (old_allocator == NULL);

        uint8_t old_bytes_buf[MAX_BUFFER_SIZE];
        uint8_t *old_bytes = NULL;
        if (old_len > 0) {
            old_bytes = old_bytes_buf;
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = cur_bytes[i];
            }
        }

        aws_string_destroy_secure(str);

        if (old_allocator_is_null) {
            const uint8_t *cur_bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(cur_bytes[i] == 0);
            }
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            assert(aws_string_is_valid(str));
        } else {
            /* memory may have been freed; no further dereference of str */
        }
    } else {
        /* str is NULL; aws_string_destroy_secure is not called because it requires a non‑null argument */
    }
}
