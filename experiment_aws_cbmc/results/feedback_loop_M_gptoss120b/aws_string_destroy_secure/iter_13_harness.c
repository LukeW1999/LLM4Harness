#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len + 1 <= MAX_BUFFER_SIZE); /* room for null terminator */

        union {
            unsigned char raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
            struct aws_string s;
        } buffer __attribute__((aligned(__alignof__(struct aws_string))));
        str = &buffer.s;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = 0; /* null‑terminate */

        aws_string_destroy_secure(str);
    } else {
        str = NULL;
        aws_string_destroy_secure(str);
    }
}
