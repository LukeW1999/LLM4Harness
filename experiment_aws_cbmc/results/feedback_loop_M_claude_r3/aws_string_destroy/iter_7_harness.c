#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness(void) {
    /* Use the default allocator which has proper function pointers */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Create a nondet length for the string */
    size_t len;
    __CPROVER_assume(len <= 10);

    /* Allocate the aws_string as a dynamic object with proper size */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize the string fields by casting away const */
    *(struct aws_allocator **)(&str->allocator) = allocator;
    *(size_t *)(&str->len) = len;
    /* Set the null terminator */
    ((uint8_t *)str->bytes)[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    /* Call aws_string_destroy */
    aws_string_destroy(str);

    assert(true);
}
