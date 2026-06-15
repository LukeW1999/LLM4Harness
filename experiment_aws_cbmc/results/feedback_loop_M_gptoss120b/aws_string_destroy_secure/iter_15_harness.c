#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len + 1 <= MAX_BUFFER_SIZE);

        unsigned char buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE]
            __attribute__((aligned(__alignof__(struct aws_string))));

        str = (struct aws_string *)buffer;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = 0;

        aws_string_destroy_secure(str);
    } else {
        str = NULL;
        aws_string_destroy_secure(str);
    }
}
