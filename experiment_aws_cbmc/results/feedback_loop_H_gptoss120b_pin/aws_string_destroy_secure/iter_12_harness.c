#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_allocator *allocator = aws_default_allocator();

        str = (struct aws_string *)aws_mem_acquire(allocator, sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        str->len = len;
        str->allocator = allocator;

        uint8_t *b = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            b[i] = nondet_uint8_t();
        }
    }

    aws_string_destroy_secure(str);
}
