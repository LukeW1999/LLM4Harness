#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    /* NULL case */
    aws_string_destroy_secure(NULL);

    /* Non-NULL case: allocate a valid string with nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    struct aws_allocator *allocator = aws_default_allocator();
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
    __CPROVER_assume(str != NULL);

    str->allocator = allocator;
    str->len = len;
    str->bytes[len] = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t val;
        str->bytes[i] = val;
    }

    aws_string_destroy_secure(str);
}
