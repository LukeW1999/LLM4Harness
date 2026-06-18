#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);

        struct {
            struct aws_string s;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } storage;

        str = &storage.s;

        struct aws_allocator *alloc = aws_default_allocator();
        str->allocator = nondet_bool() ? alloc : NULL;
        str->len = len;

        uint8_t *b = (uint8_t *)(str + 1);
        for (size_t i = 0; i < len; ++i) {
            b[i] = nondet_uint8_t();
        }
        b[len] = 0;
    }

    aws_string_destroy_secure(str);
}
