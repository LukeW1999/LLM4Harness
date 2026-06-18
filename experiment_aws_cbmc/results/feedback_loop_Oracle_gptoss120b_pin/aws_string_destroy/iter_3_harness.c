#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

static bool nondet_bool(void);
static size_t nondet_size_t(void);

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);

        uint8_t buffer[sizeof(struct aws_string) + 1024];
        str = (struct aws_string *)buffer;

        str->len = len;
        str->allocator = aws_default_allocator();

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = (uint8_t)nondet_size_t();
        }
        str->bytes[len] = '\0';
    }

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy(str);

    if (str == NULL || str->allocator == NULL) {
        assert(str == NULL || aws_string_is_valid(str));
    }
}

bool nondet_bool(void) {
    bool b;
    return b;
}

size_t nondet_size_t(void) {
    size_t s;
    return s;
}
