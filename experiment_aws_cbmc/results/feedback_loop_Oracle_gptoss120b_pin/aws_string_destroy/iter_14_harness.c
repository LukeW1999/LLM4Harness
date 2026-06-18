#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

bool nondet_bool(void);
size_t nondet_size_t(void);

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);

        uint8_t buffer[sizeof(struct aws_string) + 1024];
        str = (struct aws_string *)buffer;

        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = (uint8_t)nondet_size_t();
        }
        str->bytes[len] = (uint8_t)'\0';
    }

    aws_string_destroy(str);
}

/* nondet helpers */
bool nondet_bool(void) {
    bool b;
    return b;
}

size_t nondet_size_t(void) {
    size_t s;
    return s;
}
