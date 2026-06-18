#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool nondet_bool(void);
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        union {
            struct aws_string s;
            uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        } storage;

        str = &storage.s;
        str->allocator = aws_default_allocator();
        str->len = len;

        const uint8_t *cbytes = aws_string_bytes(str);
        uint8_t *bytes = (uint8_t *)cbytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    if (str) {
        aws_string_destroy_secure(str);
    }
}
