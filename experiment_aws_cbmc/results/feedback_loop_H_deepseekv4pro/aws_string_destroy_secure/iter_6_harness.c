#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    /* Initialize the library to set up the default allocator */
    aws_common_library_init(aws_default_allocator());

    /* NULL case */
    aws_string_destroy_secure(NULL);

    /* Non-NULL case: allocate a valid string with nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    struct aws_allocator *allocator = aws_default_allocator();
    /* Allocate memory for the string structure plus the byte buffer and null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
    /* Assume allocation succeeds (CBMC will explore both success and failure) */
    __CPROVER_assume(str != NULL);

    /* Initialize the string fields */
    str->len = len;
    str->bytes = (uint8_t *)(str + 1);
    /* Ensure the string is null-terminated */
    str->bytes[len] = 0;

    /* Optionally fill the bytes with nondet values (not required for correctness) */
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = (uint8_t)__CPROVER_nondet_uint8_t();
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);
}
