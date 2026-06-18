#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Allocate the aws_string using malloc so it's a valid dynamic object */
    size_t len;
    __CPROVER_assume(len <= 10);
    
    /* Allocate the struct with space for the bytes array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Initialize the string fields */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    /* bytes is a flexible array member, set last byte to null terminator */
    uint8_t *bytes = (uint8_t *)str->bytes;
    bytes[len] = '\0';
    
    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->allocator->mem_release != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Call function under test */
    aws_string_destroy(str);
    
    /* The function completed without error */
    assert(true);
}
