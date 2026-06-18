#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *default_allocator = aws_default_allocator();

    /* Stack‑allocated raw memory for a possible string */
    uint8_t raw_mem[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = (struct aws_string *)raw_mem;
        str->len = len;

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = default_allocator;
        }

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Preserve original state for later checks */
    size_t old_len = 0;
    struct aws_allocator *old_allocator = NULL;
    uint8_t old_buf[MAX_STRING_LEN] = {0};

    if (str) {
        old_len = str->len;
        old_allocator = str->allocator;
        if (str->len > 0) {
            const uint8_t *src = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                old_buf[i] = src[i];
            }
        }
    }

    aws_string_destroy_secure(str);

    if (str && old_allocator == NULL) {
        /* When allocator is NULL the memory is zeroed */
        assert(str->len == 0);
        assert(str->allocator == NULL);

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < old_len; ++i) {
            assert(bytes[i] == 0);
        }

        /* The zeroed string should still satisfy basic validity checks */
        assert(aws_string_is_valid(str));
    }
}
