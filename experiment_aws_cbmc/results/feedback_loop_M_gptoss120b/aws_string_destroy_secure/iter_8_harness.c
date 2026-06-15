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
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len + 1 <= MAX_BUFFER_SIZE); /* room for null terminator */

        union {
            struct aws_string s;
            uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        } u;

        str = &u.s;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        uint8_t *bytes = (uint8_t *)(str + 1);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* null‑terminate */

        aws_string_destroy_secure(str);
    } else {
        str = NULL;
        aws_string_destroy_secure(str);
    }
}
