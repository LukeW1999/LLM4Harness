#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 32

void aws_string_destroy_secure_harness(void) {
    /* Use the default allocator which has valid function pointers */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate the aws_string manually so it's a proper dynamic object */
    /* aws_string layout: allocator ptr, len, bytes[], null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Initialize fields */
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    /* bytes are nondet, just ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';

    /* Verify validity */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Call the function under test */
    aws_string_destroy_secure(str);
}
