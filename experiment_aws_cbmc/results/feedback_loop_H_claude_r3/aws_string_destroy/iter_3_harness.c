#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    /* Use the helper to create a valid aws_string, or pass NULL */
    bool is_null;

    struct aws_string *str = NULL;

    if (!is_null) {
        /* Allocate a valid aws_string using the proper structure */
        size_t len;
        __CPROVER_assume(len <= 10);

        /* Allocate enough space for the struct plus bytes plus null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* Set up the allocator field */
        struct aws_allocator *allocator = aws_default_allocator();
        *(struct aws_allocator **)&str->allocator = allocator;

        /* Set up the length field */
        *(size_t *)&str->len = len;

        /* Ensure null terminator is set - bytes is a flexible array member */
        uint8_t *bytes = (uint8_t *)str->bytes;
        bytes[len] = '\0';

        /* Ensure the string is valid before calling destroy */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    aws_string_destroy(str);
}
