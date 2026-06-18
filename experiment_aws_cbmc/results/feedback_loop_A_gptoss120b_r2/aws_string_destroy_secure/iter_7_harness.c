#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        str = (struct aws_string *)aws_mem_acquire(allocator, sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        str->allocator = allocator;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Call the function under verification. */
    aws_string_destroy_secure(str);

    /* No further dereferencing of `str` after it has been destroyed. */
}
