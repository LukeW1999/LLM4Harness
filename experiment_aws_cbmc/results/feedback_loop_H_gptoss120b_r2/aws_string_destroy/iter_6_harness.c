#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_LEN 256

bool nondet_bool(void);
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;
    struct aws_allocator *old_allocator = NULL;
    uint8_t old_bytes[MAX_LEN];
    size_t old_len = 0;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_LEN);

        /* stack‑allocated storage for the flexible‑array struct */
        uint8_t storage[sizeof(struct aws_string) + MAX_LEN];
        str = (struct aws_string *)storage;

        /* allocator may be NULL (stack) or default (heap) */
        str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }

        old_allocator = str->allocator;
        old_len = len;
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    aws_string_destroy(str);

    if (str && old_allocator) {
        /* memory would have been released; no further checks */
    } else {
        if (str) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            for (size_t i = 0; i < old_len; ++i) {
                assert(str->bytes[i] == old_bytes[i]);
            }
        }
    }
}
